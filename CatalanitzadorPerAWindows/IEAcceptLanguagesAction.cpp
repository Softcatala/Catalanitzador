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
#include "IEAcceptLanguagesAction.h"
#include "Registry.h"

IEAcceptedLanguagesAction::IEAcceptedLanguagesAction()
{
	_readVersion();	
}

wchar_t* IEAcceptedLanguagesAction::GetName()
{
	return _getStringFromResourceIDName (IDS_IEACCEPTEDLANGUAGESACTION_NAME, szName);
}

wchar_t* IEAcceptedLanguagesAction::GetDescription()
{
	return _getStringFromResourceIDName (IDS_IEACCEPTEDLANGUAGESACTION_DESCRIPTION, szDescription);	
}

bool IEAcceptedLanguagesAction::IsNeed()
{	
	wchar_t szValue[1024];
	bool bNeed = true;

	Registry registry;
	registry.OpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International", false);
	if (registry.GetString(L"AcceptLanguage", szValue, sizeof (szValue)))
	{
		if (wcsstr(szValue, L"ca-ES") != NULL)
			bNeed = false;
	}
	registry.Close();

	if (bNeed == false)
		status = AlreadyApplied;

	g_log.Log(L"IEAcceptedLanguagesAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;
}

void IEAcceptedLanguagesAction::Execute()
{
	wchar_t szValue[1024], szNew[1024];
	Registry registry;

	if (registry.OpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International", true))
	{
		if (registry.GetString(L"AcceptLanguage", szValue, sizeof (szValue)))
		{
			wcscpy_s(szNew, L"ca-ES,");
			wcscat_s(szNew, szValue);
			registry.SetString (L"AcceptLanguage", szNew);			
		}
		else
			registry.SetString(L"AcceptLanguage", L"ca-ES");
		
		status = Successful;
		registry.Close();
	}
	g_log.Log(L"IEAcceptedLanguagesAction::Execute, set AcceptLanguage %u", (wchar_t *) (status == Successful));
}

void IEAcceptedLanguagesAction::_readVersion()
{
	Registry registry;

	*szVersionAscii = 0x0;
	if (registry.OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer", false))
	{
		wchar_t szVersion[1024];

		if (registry.GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			WideCharToMultiByte(CP_ACP, 0, szVersion, wcslen(szVersion) + 1, szVersionAscii, sizeof(szVersionAscii), 
				NULL, NULL);

			g_log.Log(L"IEAcceptedLanguagesAction::_readVersion. IE version %s", szVersion);
		}		
	}	
}

char* IEAcceptedLanguagesAction::GetVersion()
{
	return szVersionAscii;
}

