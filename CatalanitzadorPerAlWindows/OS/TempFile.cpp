/* 
 * Copyright (C) 2012-2015 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "TempFile.h"

 TempFile::TempFile()
 {	
	m_created = false;
 }

TempFile::~TempFile()
{
	if (GetFileAttributes(m_filename.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename.c_str());
	}
}

wstring TempFile::GetFileName() 
{
	 _createfileIfNeeded();
	return m_filename;
}


void TempFile::_createfileIfNeeded()
{
	if (m_created)
	{
		return;
	}

	wchar_t szTempPath[MAX_PATH];
	wchar_t szFile[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, L"CAT", 0, szFile);
	DeleteFile(szFile);
	m_filename = szFile;
	m_created = true;
}

