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
#include "Registry.h"

Registry::Registry()
{
	hKey = NULL;
}

Registry::~Registry()
{
	Close();
}

bool Registry::OpenKey(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess)
{
	assert(hKey == NULL);

	return RegOpenKeyEx(hBaseKey, sSubKey, 0,
		bWriteAccess ? KEY_READ|KEY_WRITE:KEY_READ, &hKey) == ERROR_SUCCESS;
}

bool Registry::OpenKeyNoWOWRedirect(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess)
{
	assert(hKey == NULL);

	return RegOpenKeyEx(hBaseKey, sSubKey, 0,
		KEY_WOW64_64KEY | (bWriteAccess ? KEY_READ|KEY_WRITE:KEY_READ), &hKey) == ERROR_SUCCESS;
}

bool Registry::CreateKey(HKEY hBaseKey, wchar_t* sSubKey)
{
	return RegCreateKeyEx(hBaseKey, sSubKey, 0, 0, REG_OPTION_NON_VOLATILE,
						  KEY_READ | KEY_WRITE,0, &hKey,0) == ERROR_SUCCESS;
}

bool Registry::SetDWORD(wchar_t* string, DWORD value)
{	
	return RegSetValueEx(hKey,string,0, REG_DWORD, (BYTE*)&value, sizeof (DWORD)) == ERROR_SUCCESS;
}

bool Registry::GetDWORD(wchar_t* sName, DWORD *value)
{
	DWORD dwType;
	DWORD dwLen = sizeof (DWORD);	
	return RegQueryValueEx(hKey,sName,0,&dwType,(BYTE*)value,&dwLen) == ERROR_SUCCESS && dwType == REG_DWORD;
}
	
bool Registry::SetString(wchar_t* string, wchar_t* value)
{
	return RegSetValueEx(hKey,string,0, REG_SZ, (BYTE*)value, (wcslen (value) +1) * sizeof (wchar_t)) == ERROR_SUCCESS;
}

bool Registry::SetMultiString(wchar_t* string, wchar_t* value)
{
	bool bRslt;
	int len = wcslen(value);
	wchar_t* pString = new wchar_t[len + 2];

	memcpy(pString, value, (len + 1) * sizeof(wchar_t));
	pString[len+1] = NULL;

	bRslt = RegSetValueEx(hKey,string,0, REG_MULTI_SZ, (BYTE*)pString, len * sizeof(wchar_t)) == ERROR_SUCCESS;
 	delete pString;
	return bRslt;
}

bool Registry::GetString(wchar_t* sName, wchar_t* pBuffer, DWORD dwLength)
{
	DWORD dwType;
	DWORD dwLen = dwLength;
	LONG lResult = RegQueryValueEx(hKey, sName, 0, &dwType, (BYTE*)pBuffer, &dwLen);
	return RegQueryValueEx(hKey,sName,0,&dwType,(BYTE*)pBuffer,&dwLen) == ERROR_SUCCESS && dwType == REG_SZ;
}

bool Registry::Close()
{
	if (hKey != NULL)
	{
		if (RegCloseKey(hKey) == ERROR_SUCCESS)
		{
			hKey = NULL;
			return true;
		}
	}
	return false;
}


bool Registry::RegEnumKey(DWORD dwIndex, wstring& key)
{
	bool bRslt;
	wchar_t szKey[2048];
	DWORD dwSize = sizeof(szKey);

	assert(hKey != NULL);

	bRslt = RegEnumKeyEx(hKey, dwIndex, szKey, &dwSize, NULL, NULL, NULL,NULL) == ERROR_SUCCESS; 

	if (bRslt)
	{
		key = szKey;
	}
	else
	{
		key.erase();
	}
	return bRslt;
}

