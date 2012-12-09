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
#include "SlideshowThread.h"
#include "Resources.h"

#define SLIDESHOW_RSCTYPE L"SLIDESHOW_FILES"
#define INDEX_FILE L"index.html"

SlideshowThread::SlideshowThread()
{
	m_szTempDir[0] = NULL;
}

SlideshowThread::~SlideshowThread()
{
	for (unsigned int i = 0; i < m_tempFiles.size(); i++)
	{
		DeleteFile(m_tempFiles[i].c_str());
	}

	if (m_szTempDir[0] != NULL)
	{
		RemoveDirectory(m_szTempDir);
	}
}

void SlideshowThread::_createURL()
{
	wchar_t url[MAX_PATH];

	wsprintf(url, L"file:///%s\\%s", m_szTempDir, INDEX_FILE);
	m_URL = url;
}

void SlideshowThread::OnStart()
{
	vector <wstring> resources;
	wstring file;

	_createTempDirectory();
	_createURL();
	
	Resources::EnumResources(SLIDESHOW_RSCTYPE, resources);

	for (unsigned i = 0; i < resources.size(); i++)
	{
		file = m_szTempDir;
		file += L"\\";
		file += resources[i].substr(1, resources[i].size() -2);	
		Resources::DumpResource(SLIDESHOW_RSCTYPE, resources[i].c_str(), (wchar_t *)file.c_str());
		m_tempFiles.push_back(wstring(file));
	}
}

// We need to copy the file into a subdirectory to avoid conflicts with other threads
bool SlideshowThread::_createTempDirectory()
{
	wchar_t szTempDir[MAX_PATH];

	GetTempPath(MAX_PATH, szTempDir);

	// Unique temporary file (needs to create it)
	GetTempFileName(szTempDir, L"CAT", 0, m_szTempDir);
	DeleteFile(m_szTempDir);

	if (CreateDirectory(m_szTempDir, NULL) == FALSE)
	{
		g_log.Log(L"SlideshowThread::_createTempDirectory. Cannot create temp directory '%s'", m_szTempDir);
		return false;
	}
	return true;
}