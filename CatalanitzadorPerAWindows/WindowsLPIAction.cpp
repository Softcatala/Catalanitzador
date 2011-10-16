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

// There are hacks, and there are ugly hacks.
// Using a callback with timer does not allow passing an object
// we need to work statically. It is not too bad since only once instance per application
// of this property page can be run.
static ProgressStatus _progress;
static int nTotal = 3 * 60;
static int nCurrent;
static void *_data;
#define TIMER_ID 2014

WindowsLPIAction::WindowsLPIAction ()
{
	m_installed = false;
	result = NotStarted;
}

WindowsLPIAction::~WindowsLPIAction ()
{
	if (GetFileAttributes (filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile (filename);
	}	
}

wchar_t* WindowsLPIAction::GetName()
{
	return GetStringFromResourceIDName (IDS_WINDOWSLPIACTION_NAME, szName);
}

wchar_t* WindowsLPIAction::GetDescription()
{
	return GetStringFromResourceIDName (IDS_WINDOWSLPIACTION_DESCRIPTION, szDescription);
}

BOOL CALLBACK WindowsLPIAction::_enumUILanguagesProc(LPTSTR lpUILanguageString, LONG_PTR lParam)
{
	if (wcsstr (lpUILanguageString, L"ca-ES") != NULL)
	{
		WindowsLPIAction* instance = (WindowsLPIAction *) lParam; 
		instance->m_installed = true;
		return FALSE;
	}
	return TRUE;
}

wchar_t* WindowsLPIAction::_getPackageName ()
{
	OperatingVersion version = OSVersion::GetVersion ();

	switch (version)
	{
		case WindowsXP:
			return L"http://baixades.softcatala.org/?url=http://www.softcatala.org/pub/softcatala/populars/noredist/LIPsetup.msi&id=3468&mirall=softcatala2&versio=2.0&so=win32";
		case WindowsVista:		
			return L"http://baixades.softcatala.org/?url=http://www.softcatala.org/pub/softcatala/populars/noredist/lip_ca-es.mlc&id=3511&mirall=softcatala2&versio=1.0&so=win32";
		case Windows7:
			return L"http://download.microsoft.com/download/5/A/D/5ADAA4B6-C92C-43F1-8508-DB705E0E9675/LIP_ca-ES-32bit.mlc";
		default:
			break;
	}
	return NULL;
}

void WindowsLPIAction::UpdateIsInstalled()
{
	m_installed = false;

	// Checks if the Catalan language pack is already installed
	// This information is also available by checking: SYSTEM\\ControlSet001\\Control\\MUI\\UILanguages\\CA-ES
	// However, using Windows API is more standard
	EnumUILanguages(_enumUILanguagesProc, MUI_LANGUAGE_NAME, (LPARAM) this);	
}

// TODO:
//	Does not work with 64-bits Windows
//	Only works if you have Windows Spanish or French
bool WindowsLPIAction::IsNeed()
{
	bool bNeed = false;

	if (_getPackageName () != NULL)
	{
		UpdateIsInstalled();
		bNeed = (m_installed == false);
	}	

	g_log.Log (L"WindowsLPIAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool WindowsLPIAction::Download(ProgressStatus progress, void *data)
{
	InternetAccess inetacccess;
	
	GetTempPath (MAX_PATH, filename);

	OperatingVersion version = OSVersion::GetVersion ();

	if (version == WindowsXP)
	{
		wcscat_s (filename, L"lip_ca-es.msi");
	} 
	else // Windows Vista & Windows 7
	{
		wcscat_s (filename, L"lip_ca-es.mlc");
	}

	g_log.Log (L"WindowsLPIAction::Download '%s' to '%s'", _getPackageName (), filename);
	return inetacccess.GetFile (_getPackageName (), filename, progress, data);
}

// We do not really know how much time is this going to take just start by the estimation
// of nTotal and increase it when we reach the end
VOID CALLBACK WindowsLPIAction::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	nCurrent++;

	// Let's manage user expectations by increasing total by 30%
	if (nCurrent > nTotal)
		nTotal = (int) ((double) nCurrent * 1.30);

	_progress (nTotal, nCurrent, _data);
}

void WindowsLPIAction::Execute(ProgressStatus progress, void *data)
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];

	OperatingVersion version = OSVersion::GetVersion ();

	if (version == WindowsXP)
	{
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s (lpkapp, L"\\msiexec.exe ");

		wcscpy_s (szParams, L" /i "); // full path to 'lip_ca-es.msi'
		wcscat_s (szParams, filename); // full path to 'lip_ca-es.msi'
		wcscat_s (szParams, L" /qn /norestart");		
	}
	else //(version == WindowsVista) or 7
	{	
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s (szParams, L" /i ca-ES /r /s /p ");
		wcscat_s (szParams, filename);
	
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s (lpkapp, L"\\lpksetup.exe");
	}	

	result = InProgress;
	_data = data;
	_progress = progress;

	// Timer trigger every second to update progress bar
	hTimerID = SetTimer(NULL, TIMER_ID, 1000, TimerProc);

	g_log.Log (L"WindowsLPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
	runner.Execute (lpkapp, szParams);
}

bool WindowsLPIAction::DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// Once the LIP is installed the EnumUILanguages will not signal that the langpack is install until you reboot
// Additionally there are no system registry keys that indicate if the pack was installed
// The only proof at this point is checking if the directory was created
bool WindowsLPIAction::WasLIPInstalled ()
{
	wchar_t langpackDir[MAX_PATH];
	bool bExists;

	OperatingVersion version = OSVersion::GetVersion ();

	if (version == WindowsXP)
	{	
		GetSystemDirectory(langpackDir, MAX_PATH);
		wcscat_s (langpackDir, L"\\mui\\0403");
	}
	else  //(version == WindowsVista) or 7
	{
		GetWindowsDirectory(langpackDir, MAX_PATH);
		wcscat_s (langpackDir, L"\\ca-ES");
	}
		
	bExists = DirectoryExists (langpackDir);
	g_log.Log (L"WindowsLPIAction::WasLIPInstalled checking '%s' is %u", langpackDir, (wchar_t*) bExists);
	return bExists;
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void WindowsLPIAction::SetDefaultLanguage ()
{
	Registry registry;
	if (registry.OpenKey (HKEY_CURRENT_USER, L"Control Panel\\Desktop", true) == TRUE)
	{
		registry.SetString (L"PreferredUILanguages", L"ca-ES");
		registry.Close ();
		g_log.Log (L"WindowsLPIAction::SetDefaultLanguage done");
	}
}

ActionResult WindowsLPIAction::Result()
{
	if (result == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		KillTimer (NULL, hTimerID);

		if (WasLIPInstalled ()) {			
			result = Successful;			
		}
		else {
			result = FinishedWithError;			
		}
		g_log.Log (L"WindowsLPIAction::Result is '%s'", result == Successful ? L"Successful" : L"FinishedWithError");
		// TODO: Move under WasLIPInstalled
		SetDefaultLanguage ();
	}
	return result;
}

