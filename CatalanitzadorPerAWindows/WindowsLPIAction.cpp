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
#include "WindowsLPIAction.h"
#include "InternetAccess.h"

#include <string.h>
#include <stdio.h>

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
	return NULL;
}

BOOL CALLBACK WindowsLPIAction::EnumUILanguagesProc(LPTSTR lpUILanguageString, LONG_PTR lParam)
{
	if (wcsstr (lpUILanguageString, L"ca-ES") != NULL)
	{
		WindowsLPIAction* instance = (WindowsLPIAction *) lParam; 
		instance->m_installed = true;
		return FALSE;
	}
	return TRUE;
}

bool WindowsLPIAction::IsNeed()
{
	EnumUILanguages(EnumUILanguagesProc, MUI_LANGUAGE_NAME, (LPARAM) this);
	return true;
	//return m_installed;
}

bool WindowsLPIAction::Download()
{
	InternetAccess inetacccess;

	inetacccess.GetFile (
		L"http://baixades.softcatala.org/?url=http://www.softcatala.org/pub/softcatala/populars/noredist/LIPsetup.msi&id=3468&mirall=softcatala2&versio=2.0&so=win32",
		L"LIPsetup.msi");

	return true;
}

void WindowsLPIAction::Execute()
{
}

void WindowsLPIAction::Result()
{

}

