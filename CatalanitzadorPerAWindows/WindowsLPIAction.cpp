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

WindowsLPIAction::WindowsLPIAction ()
{
	m_installed = false;
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
		default:
			break;
	}
	return NULL;
}


bool WindowsLPIAction::IsNeed()
{
	if (_getPackageName () == NULL)
		return false;

	// TODO:
	//	Does not work with 64-bits Windows
	//	Only works if you have Windows Spanish or French

	EnumUILanguages(_enumUILanguagesProc, MUI_LANGUAGE_NAME, (LPARAM) this);
	return m_installed == false;
}

static wchar_t filename[MAX_PATH];

bool WindowsLPIAction::Download()
{
	InternetAccess inetacccess;

	GetTempPath (MAX_PATH, filename);
	wcscat_s (filename, L"lip_ca-es.mlc");

	return inetacccess.GetFile (_getPackageName (), filename);
}

void WindowsLPIAction::Execute()
{
	wchar_t szParams[MAX_PATH];
	Runner runner;
	
	// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
	wcscpy_s (szParams, L" /i ca-ES /r /s /p ");
	wcscat_s (szParams, filename);

	runner.Execute (L"c:\\windows\\system32\\lpksetup.exe",
		szParams);
}

void WindowsLPIAction::Result()
{

}

