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
#include "Url.h"

Url::Url(wchar_t* url)
{
	_extractpath(url);
}

void Url::_extractpath(wchar_t* url)
{
	BOOL rslt;
	wchar_t szPath[1024];
	wchar_t szExtraInfo[1024];

	memset(&m_components,0, sizeof(m_components));
	m_components.dwStructSize = sizeof(m_components);

	m_components.lpszUrlPath = szPath;
	m_components.dwUrlPathLength = sizeof(szPath);
	m_components.lpszExtraInfo = szExtraInfo;
	m_components.dwExtraInfoLength = sizeof(szExtraInfo);

	rslt = InternetCrackUrl(url, wcslen(url), ICU_ESCAPE, &m_components);

	if (rslt == TRUE)
	{
		_extractfilename(szPath);
	}
	else
		m_filename[0] = L'\0';
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