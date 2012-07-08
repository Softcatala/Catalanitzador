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
#include "FileVersionInfo.h"

FileVersionInfo::FileVersionInfo(wstring file)
{
	m_file = file;
	m_majorVersion = -1;
	m_languageCode = -1;
}

wstring& FileVersionInfo::GetVersion()
{	
	if (m_version.size() == 0)
	{
		_readVersion();
	}

	return m_version;		
}

int FileVersionInfo::GetMajorVersion()
{
	if (m_majorVersion == -1)
	{
		_readVersion();
	}

	return m_majorVersion;
}

DWORD FileVersionInfo::GetLanguageCode()
{
	if (m_languageCode == -1)
	{
		_readLanguageCode();
	}

	return m_languageCode;
}

void FileVersionInfo::_readVersion()
{	
	DWORD dwLen, dwUnUsed;
	LPTSTR lpVI = NULL;	
	
	dwLen = GetFileVersionInfoSize(m_file.c_str(), &dwUnUsed);

	if (dwLen > 0)
	{
		lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);
	}

	if (lpVI != NULL)
	{		
		VS_FIXEDFILEINFO *lpFfi;
		wchar_t szBuffer[2048];
		UINT uLen = 0;

		GetFileVersionInfo(m_file.c_str(), NULL, dwLen, lpVI);

		if (VerQueryValue(lpVI , L"\\" , (LPVOID *)&lpFfi , &uLen))
		{
			swprintf_s(szBuffer, L"%d.%d.%d.%d", HIWORD(lpFfi->dwProductVersionMS), LOWORD(lpFfi->dwProductVersionMS), 
				HIWORD(lpFfi->dwProductVersionLS), LOWORD(lpFfi->dwProductVersionLS));
			m_version = szBuffer;
			m_majorVersion =  HIWORD(lpFfi->dwProductVersionMS);
		}

		GlobalFree((HGLOBAL)lpVI);
	}

	g_log.Log(L"FileVersionInfo::ReadVersion. File '%s' version '%s'", (wchar_t*) m_file.c_str(), (wchar_t*) m_version.c_str());	
}


struct LANGCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} *lpTranslate;

void FileVersionInfo::_readLanguageCode()
{
	DWORD dwLen, dwUnUsed;
	LPTSTR lpVI = NULL;

	dwLen = GetFileVersionInfoSize(m_file.c_str(), &dwUnUsed);

	if (dwLen > 0)
	{
		lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);
	}	

	if (lpVI != NULL)
	{		
		GetFileVersionInfo(m_file.c_str(), NULL, dwLen, lpVI);
		unsigned int uLen;

		VerQueryValue(lpVI,
			L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate,  &uLen);
		
		m_languageCode = lpTranslate->wLanguage;
		GlobalFree((HGLOBAL)lpVI);
	}

	g_log.Log(L"FileVersionInfo::_readLanguageCode. File '%s' language '%u'", (wchar_t*) m_file.c_str(), (wchar_t*) m_languageCode);	
}
