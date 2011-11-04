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
	Close ();
}

bool Registry::OpenKey(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess)
{
	return RegOpenKeyEx(hBaseKey, sSubKey, 0,
		bWriteAccess ? KEY_READ|KEY_WRITE:KEY_READ, &hKey) == ERROR_SUCCESS;
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