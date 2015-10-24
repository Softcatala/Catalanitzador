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
#include "Resources.h"

bool Resources::DumpResource(LPCWSTR type, LPCWSTR resource, wchar_t* file)
{
	HRSRC hRsrc = NULL;
	HGLOBAL hGlbl = NULL;
	BYTE *pExeResource = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD size;
	HMODULE hInstance;

	hInstance = GetModuleHandle(NULL);

	hRsrc = FindResource(hInstance, resource, type);

	if (hRsrc == NULL)
 		return false;

	size = SizeofResource(hInstance, hRsrc);

	hGlbl = LoadResource(hInstance, hRsrc);
	if (hGlbl == NULL)
		return false;

	pExeResource = (BYTE*)LockResource(hGlbl);
	if (pExeResource == NULL)
		return false;
   
	hFile = CreateFile(file, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		g_log.Log(L"Resources::DumpResource. Failed to dump to '%s'", file);
		return false;
	}
	else
	{
		BYTE byte = 0;
		DWORD bytesWritten = 0;
		for (DWORD i=0; i<size; i++)
		{
			byte = pExeResource[i];
			WriteFile(hFile, &byte, sizeof(byte), &bytesWritten, NULL);
		}
		CloseHandle(hFile);
		return true;
	}
}

#define UNICODE_MARK_SIZE 2

bool Resources::LoadResourceToString(LPCWSTR type, LPCWSTR resource, wstring& text)
{
	HRSRC hRsrc = NULL;
	HGLOBAL hGlbl = NULL;
	BYTE *pExeResource = NULL;
	DWORD size;
	HMODULE hInstance;

	hInstance = GetModuleHandle(NULL);

	hRsrc = FindResource(hInstance, resource, type);

	if (hRsrc == NULL)
 		return false;

	size = SizeofResource(hInstance, hRsrc);

	hGlbl = LoadResource(hInstance, hRsrc);
	if (hGlbl == NULL)
		return false;

	pExeResource = (BYTE*)LockResource(hGlbl);
	if (pExeResource == NULL)
		return false;	

	text = (wchar_t *) (pExeResource + UNICODE_MARK_SIZE);
	return true;
}

BOOL Resources::EnumResNameProcCallback(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
	vector <wstring>* resources;

	resources = (vector <wstring>*) lParam;
	resources->push_back(wstring(lpszName));
	return TRUE;
}

bool Resources::EnumResources(LPCWSTR type, vector <wstring>& resources)
{
	HMODULE hInstance;

	hInstance = GetModuleHandle(NULL);
	return EnumResourceNames(hInstance, type, EnumResNameProcCallback, (LONG_PTR) &resources) == TRUE;	
}  

