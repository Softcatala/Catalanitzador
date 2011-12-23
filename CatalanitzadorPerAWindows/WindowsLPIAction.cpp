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

#include "WindowsLPIAction.h"
#include "InternetAccess.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Registry.h"
#include "Url.h"

// There are hacks, and there are ugly hacks.
// Using a callback with timer does not allow passing an object
// we need to work statically. It is not too bad since only once instance per application
// of this property page can be run.
static ProgressStatus _progress;
static int nTotal = 3 * 60;
static int nCurrent;
static void *_data;
#define TIMER_ID 2014

WindowsLPIAction::WindowsLPIAction()
{	
	filename[0] = NULL;
	CheckPrerequirements();
}

WindowsLPIAction::~WindowsLPIAction()
{
	if (filename[0] != NULL  && GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(filename);
	}
}

wchar_t* WindowsLPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NAME, szName);
}

wchar_t* WindowsLPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_DESCRIPTION, szDescription);
}

wchar_t* WindowsLPIAction::_getPackageName()
{
	OperatingVersion version = OSVersion::GetVersion();

	switch (version)
	{
		case WindowsXP:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/WindowsXP/Build2006Professional/LIPsetup.msi";
		case WindowsVista:		
			return L"http://www.softcatala.org/pub/beta/catalanitzador/WindowsVista/lip_ca-es.mlc";
		case Windows7:
			return L"http://www.softcatala.org/pub/beta/catalanitzador/Windows7/LIP_ca-ES-32bit.mlc";
		default:
			break;
	}
	return NULL;
}

// Checks if the Catalan language pack is already installed
// This code works if the langpack is installed or has just been installed (and the user did not reboot)
bool WindowsLPIAction::_isLangPackInstalled()
{	
	wchar_t langpackDir[MAX_PATH];
	Registry registry;
	bool bExists;

	OperatingVersion version = OSVersion::GetVersion();

	if (version == WindowsXP)
	{
		bExists = false;
		if (registry.OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\", false))
		{
			wchar_t szValue[1024];		

			// MultiUILanguageId key is left behind
			if (registry.GetString(L"MUILanguagePending", szValue, sizeof (szValue)))
			{
				if (wcsstr(szValue, L"0403") != NULL)
					bExists = true;
			}

			registry.Close();
		}		

	}
	else  //(version == WindowsVista) or 7
	{		
		bExists = registry.OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES", false);
		registry.Close();
	}		
	
	g_log.Log (L"WindowsLPIAction::_updateIsInstalled checking is %u", (wchar_t*) bExists);
	return bExists;
}

// TODO:
//	Does not work with 64-bits Windows
//	Only works if you have Windows Spanish or French
bool WindowsLPIAction::IsNeed()
{
	if (status == CannotBeApplied)
		return false;
	
	bool bNeed = false;

	if (_getPackageName() != NULL)
	{		
		if (_isLangPackInstalled() == false)
		{		
			bNeed = true;
		}
		{
			status = AlreadyApplied;
		}
	}
	else
	{
		status = CannotBeApplied;
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::IsNeed. Unsupported Windows version");
	}

	g_log.Log(L"WindowsLPIAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool WindowsLPIAction::Download(ProgressStatus progress, void *data)
{
	InternetAccess inetacccess;

	GetTempPath(MAX_PATH, filename);

	OperatingVersion version = OSVersion::GetVersion();
	Url url (_getPackageName());
	wcscat_s (filename, url.GetFileName());
	
	g_log.Log (L"WindowsLPIAction::Download '%s' to '%s'", _getPackageName(), filename);
	return inetacccess.GetFile (_getPackageName(), filename, progress, data);
}

// We do not really know how much time is this going to take just start by the estimation
// of nTotal and increase it when we reach the end
VOID CALLBACK WindowsLPIAction::_timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	nCurrent++;

	// Let's manage user expectations by increasing total by 30%
	if (nCurrent > nTotal)
		nTotal = (int) ((double) nCurrent * 1.30);

	_progress(nTotal, nCurrent, _data);
}

void WindowsLPIAction::Execute(ProgressStatus progress, void *data)
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];

	OperatingVersion version = OSVersion::GetVersion();

	if (version == WindowsXP)
	{
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\msiexec.exe ");

		wcscpy_s(szParams, L" /i ");
		wcscat_s(szParams, filename); // full path to 'lip_ca-es.msi'
		wcscat_s(szParams, L" /qn /norestart");
	}
	else // Windows Vista and 7
	{	
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s(szParams, L" /i ca-ES /r /s /p ");
		wcscat_s(szParams, filename);
	
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\lpksetup.exe");
	}	

	status = InProgress;
	_data = data;
	_progress = progress;

	// Timer trigger every second to update progress bar
	hTimerID = SetTimer(NULL, TIMER_ID, 1000, _timerProc);

	g_log.Log(L"WindowsLPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
	runner.Execute(lpkapp, szParams);
}

bool WindowsLPIAction::_directoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void WindowsLPIAction::_setDefaultLanguage()
{
	if (OSVersion::GetVersion() == WindowsXP)
		return;

	Registry registry;
	if (registry.OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", true) == TRUE)
	{
		registry.SetString(L"PreferredUILanguages", L"ca-ES");
		registry.Close();
		g_log.Log(L"WindowsLPIAction::_setDefaultLanguage done");
	}
}

ActionStatus WindowsLPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		KillTimer(NULL, hTimerID);

		if (_isLangPackInstalled()) {
			status = Successful;
			_setDefaultLanguage();
		}
		else {
			status = FinishedWithError;			
		}
		
		g_log.Log(L"WindowsLPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool WindowsLPIAction::IsRebootNeed()
{
	return status == Successful;
}

#define SPANISH_LOCALEID 0x0A
#define FRENCH_LOCALEID 0x0c

void WindowsLPIAction::CheckPrerequirements() 
{
	LANGID langid;

	langid = GetSystemDefaultUILanguage() & 0xf;

	if (langid != SPANISH_LOCALEID && langid != FRENCH_LOCALEID)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NOSPFR, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::CheckPrerequirements. No Spanish or French Windows found");
		status = CannotBeApplied;
	}
}


