﻿/* 
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
#include "Url.h"

Url::Url(wstring string) 
{
	_extractpath((wchar_t *)string.c_str());
}

Url::Url(wchar_t* url)
{
	_extractpath(url);
}

void Url::_extractpath(wchar_t* url)
{
	BOOL rslt;	
	wchar_t szExtraInfo[1024];

	memset(&m_components,0, sizeof(m_components));
	m_components.dwStructSize = sizeof(m_components);

	m_components.lpszHostName = m_hostname;
	m_components.dwHostNameLength = sizeof(m_hostname);
	m_components.lpszUrlPath = m_path;
	m_components.dwUrlPathLength = sizeof(m_path);
	m_components.lpszExtraInfo = szExtraInfo;
	m_components.dwExtraInfoLength = sizeof(szExtraInfo);
	m_components.lpszScheme = m_lpszScheme;
	m_components.dwSchemeLength = sizeof(m_lpszScheme);

	rslt = InternetCrackUrl(url, url != NULL ? wcslen(url) : 0, ICU_ESCAPE, &m_components);

	if (rslt == TRUE)
	{
		_extractfilename(m_path);
	}
	else
	{
		m_filename[0] = L'\0';
		m_hostname[0] = L'\0';
		m_path[0] = L'\0';
	}
}

void Url::_extractfilename(wchar_t* path)
{
	wchar_t* pos_path;
	wchar_t* pos_end;	
	
	pos_end = pos_path = path + wcslen(path);
	
	while (*(pos_path -1) != L'/' && pos_path >= path)
	{
		pos_path--;
	}

	int len = pos_end - pos_path;
	wcsncpy_s(m_filename, pos_path, len);
	m_filename[len+1] = L'\0';
}

// See: http://www.w3.org/International/O-URL-code.html
void Url::EncodeParameter(wstring parameter, wstring& encoded)
{
	char szUTF8[2048];	

	WideCharToMultiByte(CP_UTF8, 0, parameter.c_str(), parameter.size() + 1,
		szUTF8, sizeof(szUTF8), 0, 0);

	for (unsigned int i = 0; i < strlen(szUTF8); i++)
	{
		unsigned char c;

		c = szUTF8[i];

		if (c =='.' ||
			(c >='0' && c <='9') ||
			(c >='A' && c <='Z') ||
			(c >='a' && c <='z'))
		{
			encoded += c;
		}		
		else
		{
			wchar_t szString[16];

			wsprintf(szString, L"%%%X", c);
			encoded += szString;
		}	
	}	
}