/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include <windows.h>
#include "Registry.h"


BOOL CALLBACK _enumUILanguagesProc(LPTSTR lpUILanguageString, LONG_PTR lParam)
{
	wprintf(L"\t\tLangID: %s\r\n", lpUILanguageString);
	return TRUE;
}

void ShowUserInterface()
{
	LANGID sysLangID;

	wprintf(L"User Interface\r\n");
	sysLangID = GetSystemDefaultUILanguage();
	wprintf(L"\tGetSystemDefaultUILanguage: %x\r\n", sysLangID);

	wprintf(L"\tEnumUILanguages\r\n");
	::EnumUILanguages(_enumUILanguagesProc, MUI_LANGUAGE_ID, NULL);	
}

void ShowLocale()
{
	wchar_t szValue[1024] = L"";
	Registry registry;

	wprintf(L"Regional Settings\r\n");
	if (registry.OpenKey(HKEY_CURRENT_USER, L"Control Panel\\International", false))
	{
		registry.GetString(L"Locale", szValue, sizeof (szValue));
		registry.Close();
	}
	wprintf(L"\tLocale: %s\r\n", szValue);
}

void ShowInstalledKeyboards()
{
	wchar_t szValue[1024];
	wchar_t szNum[16];
	Registry registry;
	
	wprintf(L"Installed keyboards\r\n");
	if (registry.OpenKey(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", false))
	{
		for (int i = 1; i < 100; i++)
		{
			swprintf_s(szNum, L"%u", i);
			if (registry.GetString(szNum, szValue, sizeof (szValue)) == false)
				break;

			wprintf(L"\tLanguage %s\n\r", szValue);
		}			
		registry.Close();
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	ShowUserInterface();
	ShowLocale();
	ShowInstalledKeyboards();
	return 0;
}

