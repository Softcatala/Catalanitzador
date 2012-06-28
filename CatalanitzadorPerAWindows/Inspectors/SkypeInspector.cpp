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
#include "SkypeInspector.h"


SkypeInspector::SkypeInspector(IRegistry* registry)
{
	m_registry = registry;
}

#define PROGRAM_REGKEY L"Software\\Skype\\Phone\\UI\\General"

void SkypeInspector::_readLangInstalled()
{
	wchar_t szLanguage[1024] = L"";

	if (m_registry->OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, false))
	{
		m_registry->GetString(L"Language", szLanguage, sizeof(szLanguage));
		m_registry->Close();
	}

	g_log.Log(L"SkypeInspector::_readLangInstalled '%s'", szLanguage);
	m_KeyValues.push_back(InspectorKeyValue(L"lang", szLanguage));
}

void SkypeInspector::Execute()
{
	_readVersion();
	_readLangInstalled();	
}

bool SkypeInspector::_readFilePath(wstring &path)
{
	wchar_t szFile[1024];

	path.empty();
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Software\\Skype\\Phone", false))
	{
		if (m_registry->GetString(L"SkypePath", szFile, sizeof(szFile)))
		{
			path = szFile;
		}
		m_registry->Close();
	}	
	g_log.Log(L"SkypeInspector::_readFilePath '%s'", (wchar_t *) path.c_str());
	return path.size() > 0;
}

bool SkypeInspector::_readVersion()
{	
	wstring file;
	DWORD dwLen, dwUnUsed;
	LPTSTR lpVI = NULL;
	wstring version;

	_readFilePath(file);	
	dwLen = GetFileVersionInfoSize(file.c_str(), &dwUnUsed);

	if (dwLen > 0)
	{
		lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);
	}

	if (lpVI != NULL)
	{		
		VS_FIXEDFILEINFO *lpFfi;
		wchar_t szBuffer[2048];
		UINT uLen = 0;

		GetFileVersionInfo(file.c_str(), NULL, dwLen, lpVI);

		if (VerQueryValue(lpVI , L"\\" , (LPVOID *)&lpFfi , &uLen))
		{
			swprintf_s(szBuffer, L"%d.%d.%d.%d", HIWORD(lpFfi->dwProductVersionMS), LOWORD(lpFfi->dwProductVersionMS), 
				HIWORD(lpFfi->dwProductVersionLS), LOWORD(lpFfi->dwProductVersionLS));
			version = szBuffer;
		}

		GlobalFree((HGLOBAL)lpVI);
	}

	g_log.Log(L"SkypeInspector::_readVersion version %s", (wchar_t*) version.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"version", version.c_str()));
	return version.size() > 0;
}
