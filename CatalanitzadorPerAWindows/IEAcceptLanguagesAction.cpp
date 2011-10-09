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

// See: HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\International

wchar_t* IEAcceptedLanguagesAction::GetName()
{
	return GetStringFromResourceIDName (IDS_IEACCEPTEDLANGUAGESACTION_NAME, szName);	
}

wchar_t* IEAcceptedLanguagesAction::GetDescription()
{
	return GetStringFromResourceIDName (IDS_IEACCEPTEDLANGUAGESACTION_DESCRIPTION, szDescription);	
}

bool getRegistryValue(HKEY hKey,
					  LPCTSTR sName,
					  LPBYTE pBuffer,
					  int dwLength)
{
	DWORD dwType;
	DWORD dwLen = dwLength;
	LONG lResult = RegQueryValueEx(hKey,sName,0,&dwType,(BYTE*)pBuffer,&dwLen);
	return RegQueryValueEx(hKey,sName,0,&dwType,(BYTE*)pBuffer,&dwLen) == ERROR_SUCCESS && dwType == REG_SZ;
}

bool IEAcceptedLanguagesAction::IsNeed()
{
	HKEY hKey;	
	TCHAR value[255];
	bool hasCatalan = false;
	 
	if(RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\International",0,KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if(getRegistryValue(hKey,L"AcceptLanguage", (LPBYTE) value,  sizeof (value)) != 0)
		{
			if (strstr ((char *) value, "ca-ES") != NULL)
				hasCatalan = true;
		}

		RegCloseKey(hKey);
	}	
	return hasCatalan == false;
}


void IEAcceptedLanguagesAction::Execute()
{

}

ActionResult IEAcceptedLanguagesAction::Result()
{
	return Successfull;
}

