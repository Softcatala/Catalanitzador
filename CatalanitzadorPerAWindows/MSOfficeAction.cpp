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
	GetTempPath(MAX_PATH, szTempPath);
}

MSOfficeAction::~MSOfficeAction()
{
	if (filename[0] != NULL  && GetFileAttributes (filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile (filename);
	}

	// This deletes the contents of the extracted CAB file for MS Office 2003
	if (m_MSVersion != MSOffice2003)
	{
		wchar_t szFile[MAX_PATH];
		wchar_t* files[] = {L"DW20.ADM_1027", L"DWINTL20.DLL_0001_1027", L"LIP.MSI",
			L"lip.xml", L"OSE.EXE", L"SETUP.CHM_1027", L"SETUP.EXE", L"SETUP.INI", L"lip1027.cab",
			NULL};

		for(int i = 0; files[i] != NULL; i++)
		{
			wcscpy_s(szFile, szTempPath);
			wcscat_s(szFile, files[i]);

			if (GetFileAttributes (szFile) != INVALID_FILE_ATTRIBUTES)
			{
				DeleteFile(szFile);
			}
		}
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

bool MSOfficeAction::_isLangPackForVersionInstalled(wchar_t* szKey)
{
	Registry registry;
	wchar_t szValue[1024];	
	bool Installed = false;

	if (registry.OpenKey(HKEY_LOCAL_MACHINE, szKey, false))
	{
		if (registry.GetString(L"1027", szValue, sizeof (szValue)))
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

	g_log.Log(L"MSOfficeAction::_getVersionInstalledWithNoLangPack '%u'", (wchar_t *) m_MSVersion);
}

wchar_t* MSOfficeAction::_getPackageName()
{
	switch (m_MSVersion)
	{
		case MSOffice2010:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe";
		case MSOffice2007:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/office2007/office2007-lip.exe";
		case MSOffice2003:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/office2003/office2003-lip.cab";
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
	
	wcscpy_s(filename, szTempPath);

	switch (m_MSVersion)
	{
	case MSOffice2010:
		wcscat_s(filename, L"LanguageInterfacePack-x86-ca-es.exe");
		break;
	case MSOffice2007:
		wcscat_s(filename, L"office2007-lip.exe");
		break;
	case MSOffice2003:
		wcscat_s(filename, L"office2003-lip.cab");
		break;
	default:
		break;
	}
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

bool MSOfficeAction::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");

	swprintf_s (szParams, L" %s %s -f:*", filename, path);
	g_log.Log(L"MSOfficeAction::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute (szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

void MSOfficeAction::Execute(ProgressStatus progress, void *data)
{
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];

	switch (m_MSVersion)
	{
		case MSOffice2010:
		{
			wcscpy_s(szApp, L"office2003-lip.exe");
			wcscpy_s(szParams, L" /passive /norestart /quiet");
			break;
		}
		case MSOffice2007:
		{
			wcscpy_s(szApp, L"office2007-lip.exe");
			wcscpy_s(szParams, L" /quiet");
			break;
		}
		case MSOffice2003:
		{
			wchar_t szMSI[MAX_PATH];
		
			_extractCabFile(filename, szTempPath);

			GetSystemDirectory(szApp, MAX_PATH);
			wcscat_s(szApp, L"\\msiexec.exe ");

			wcscpy_s(szMSI, szTempPath);
			wcscat_s(szMSI, L"lip.msi");

			wcscpy_s(szParams, L" /i ");
			wcscat_s(szParams, szMSI);
			wcscat_s(szParams, L" /passive /norestart /quiet");			
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

	g_log.Log(L"MSOfficeAction::Execute '%s' with params '%s'", szApp, szParams);
	runner.Execute (szApp, szParams);	
}

bool MSOfficeAction::_wasInstalledCorrectly()
{	
	switch (m_MSVersion)
	{
	case MSOffice2010:
		return _isLangPackForVersionInstalled(L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs");
	case MSOffice2007:
		return _isLangPackForVersionInstalled(L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\InstalledUIs");
	case MSOffice2003:
		return _isLangPackForVersionInstalled(L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources");
	default:
		return false;
	}
}

void MSOfficeAction::_setDefaultLanguage()
{
	wchar_t szKeyName [1024];

	switch (m_MSVersion)
	{
		case MSOffice2003:
			wcscpy(szKeyName, L"Software\\Microsoft\\Office\\11.0\\Common\\LanguageResources");
			break;
		case MSOffice2007:
			wcscpy(szKeyName, L"Software\\Microsoft\\Office\\12.0\\Common\\LanguageResources");
			break;
		case MSOffice2010: // Do nothing
			return;
	}

	Registry registry;
	BOOL bSetKey = FALSE;

	if (registry.OpenKey(HKEY_CURRENT_USER, szKeyName, true))
	{
		registry.SetDWORD(L"UILanguage", 1027);
		bSetKey = TRUE;
		registry.Close();
	}
	g_log.Log(L"MSOfficeAction::_setDefaultLanguage, set AcceptLanguage %u", (wchar_t *) bSetKey);	
}

ActionResult MSOfficeAction::Result()
{
	if (result == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		KillTimer(NULL, hTimerID);

		if (_wasInstalledCorrectly()) {
			result = Successful;
		}
		else {
			result = FinishedWithError;
		}
		_setDefaultLanguage();
		g_log.Log(L"MSOfficeAction::Result is '%s'", result == Successful ? L"Successful" : L"FinishedWithError");
	}
	return result;
}
