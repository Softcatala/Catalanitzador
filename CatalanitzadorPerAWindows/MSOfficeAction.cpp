/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#include "stdafx.h"
#include <string.h>
#include <stdio.h>

#include "MSOfficeAction.h"
#include "InternetAccess.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Registry.h"

// There are hacks, and there are ugly hacks.
// Using a callback with timer does not allow passing an object
// we need to work statically. It is not too bad since only once instance per application
// of this property page can be run.
static ProgressStatus _progress;
static int nTotal = 3 * 60;
static int nCurrent;
static void *_data;
#define TIMER_ID 2014

#define OFFICE_2003_VERSION L"11.0"
#define OFFICE_2007_VERSION L"12.0"
#define OFFICE_2010_VERSION L"14.0"

MSOfficeAction::MSOfficeAction()
{	
	result = NotStarted;
	filename[0] = NULL;	
	_getVersionInstalledWithNoLangPack ();
}

MSOfficeAction::~MSOfficeAction()
{
	if (filename[0] != NULL  && GetFileAttributes (filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile (filename);
	}	
}

wchar_t* MSOfficeAction::GetName()
{
	return GetStringFromResourceIDName (IDS_MSOFFICEACTION_NAME, szName);
}

wchar_t* MSOfficeAction::GetDescription()
{
	return GetStringFromResourceIDName (IDS_MSOFFICEACTION_DESCRIPTION, szDescription);
}

bool MSOfficeAction::_isVersionInstalled (wchar_t* version)
{
	Registry registry;
	wchar_t szValue[1024];
	wchar_t szKey[1024];
	bool Installed = false;

	swprintf_s (szKey, L"SOFTWARE\\Microsoft\\Office\\%s\\Common\\InstallRoot", version);
	if (registry.OpenKey (HKEY_LOCAL_MACHINE, szKey, false))
	{
		if (registry.GetString (L"Path", szValue, sizeof (szValue)))
		{
			if (wcslen (szValue) > 0)
				Installed = true;
		}
		registry.Close ();
	}
	return Installed;
}

bool MSOfficeAction::_isLangPackForVersionInstalled(wchar_t* version)
{
	Registry registry;
	wchar_t szValue[1024];
	wchar_t szKey[1024];
	bool Installed = false;

	swprintf_s (szKey, L"SOFTWARE\\Microsoft\\Office\\%s\\Common\\LanguageResources\\InstalledUIs", version);
	if (registry.OpenKey (HKEY_LOCAL_MACHINE, szKey, false))
	{
		if (registry.GetString (L"1027", szValue, sizeof (szValue)))
		{
			if (wcslen (szValue) > 0)
				Installed = true;
		}
		registry.Close ();
	}
	return Installed;
}

bool MSOfficeAction::_isVersionInstalledWithNoLangPack(wchar_t* version)
{
	if (_isVersionInstalled (version) && _isLangPackForVersionInstalled (version) == false)
		return true;

	return false;
}

void MSOfficeAction::_getVersionInstalledWithNoLangPack()
{
	if (_isVersionInstalledWithNoLangPack(OFFICE_2010_VERSION))
		m_MSVersion = MSOffice2010;
	else if (_isVersionInstalledWithNoLangPack(OFFICE_2007_VERSION))
		m_MSVersion = MSOffice2007;
	else if (_isVersionInstalledWithNoLangPack(OFFICE_2003_VERSION))
		m_MSVersion =  MSOffice2003;
	else
		m_MSVersion = NoMSOffice;	
}

wchar_t* MSOfficeAction::_getPackageName()
{
	switch (m_MSVersion)
	{
		case MSOffice2010:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe";
		case MSOffice2003:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/office2003/office2003-lip.exe";
		default:
			break;
	}
	return NULL;
}

bool MSOfficeAction::IsNeed()
{
	bool bNeed;

	bNeed = m_MSVersion != NoMSOffice;
	g_log.Log(L"MSOfficeAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool MSOfficeAction::Download(ProgressStatus progress, void *data)
{
	InternetAccess inetacccess;
	
	GetTempPath(MAX_PATH, filename);
	wcscat_s(filename, L"LanguageInterfacePack-x86-ca-es.exe");
	g_log.Log(L"MSOfficeAction::Download '%s' to '%s'", _getPackageName (), filename);
	return inetacccess.GetFile (_getPackageName (), filename, progress, data);
}

// We do not really know how much time is this going to take just start by the estimation
// of nTotal and increase it when we reach the end
VOID CALLBACK MSOfficeAction::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	nCurrent++;

	// Let's manage user expectations by increasing total by 30%
	if (nCurrent > nTotal)
		nTotal = (int) ((double) nCurrent * 1.30);

	_progress (nTotal, nCurrent, _data);
}

void MSOfficeAction::Execute(ProgressStatus progress, void *data)
{
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];

	switch (m_MSVersion)
	{
	case MSOffice2010:
		{
			wcscpy_s (szApp, L"office2003-lip.exe");
			wcscpy_s (szParams, L" /passive /norestart /quiet");
			break;
		}
	case MSOffice2003:
		{
			wcscpy_s (szApp, filename);
			wcscpy_s (szParams, L" /q");			
			break;
		}
	default:
		break;
	}

	result = InProgress;
	_data = data;
	_progress = progress;

	// Timer trigger every second to update progress bar
	hTimerID = SetTimer(NULL, TIMER_ID, 1000, TimerProc);

	g_log.Log (L"MSOfficeAction::Execute '%s' with params '%s'", szApp, szParams);
	runner.Execute (szApp, szParams);	
}

bool MSOfficeAction::_wasInstalledCorrectly()
{	
	switch (m_MSVersion)
	{
	case MSOffice2010:
		return _isLangPackForVersionInstalled(OFFICE_2010_VERSION);
	case MSOffice2007:
		return _isLangPackForVersionInstalled(OFFICE_2007_VERSION);
	case MSOffice2003:
		return _isLangPackForVersionInstalled(OFFICE_2003_VERSION);
	default:
		return false;
	}
}

ActionResult MSOfficeAction::Result()
{
	if (result == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		KillTimer (NULL, hTimerID);

		if (_wasInstalledCorrectly()) {
			result = Successful;
		}
		else {
			result = FinishedWithError;
		}
		g_log.Log(L"MSOfficeAction::Result is '%s'", result == Successful ? L"Successful" : L"FinishedWithError");
	}
	return result;
}
