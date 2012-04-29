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
#include "Slideshow.h"
#include "Resources.h"

#define SLIDESHOW_RSCTYPE L"SLIDESHOW_FILES"
#define INDEX_FILE L"index.html"

Slideshow::Slideshow()
{
	m_hThread = NULL;
	_createURL();
}

Slideshow::~Slideshow()
{
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	for (unsigned int i = 0; i < m_tempFiles.size(); i++)
	{
		DeleteFile(m_tempFiles[i].c_str());
	}
}

void Slideshow::_createURL()
{
	wchar_t tempPath[MAX_PATH], url[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);

	wsprintf(url, L"file:///%s%s", tempPath, INDEX_FILE);
	m_URL = url;
}

void Slideshow::_unpackSlideShow()
{
	vector <wstring> resources;
	wchar_t tempPath[MAX_PATH];
	wstring file;

	GetTempPath(MAX_PATH, tempPath);
	Resources::EnumResources(SLIDESHOW_RSCTYPE, resources);

	for (unsigned i = 0; i < resources.size(); i++)
	{
		file = tempPath;
		file += resources[i].substr(1, resources[i].size() -2);	
		Resources::DumpResource(SLIDESHOW_RSCTYPE, resources[i].c_str(), (wchar_t *)file.c_str());
		m_tempFiles.push_back(wstring(file));
	}
}

DWORD Slideshow::_unpackThread(LPVOID lpParam)
{
	Slideshow* stats = (Slideshow *) lpParam;
	stats->_unpackSlideShow();
	return 0;
}

void Slideshow::StartUnpackThread()
{
	m_hThread = CreateThread(NULL, 0, _unpackThread, this, 0, NULL);
}

void Slideshow::WaitForThread()
{
	// In case the user exists the app very quickly, give time the upload to complete
	if (m_hThread != NULL)
		WaitForSingleObject(m_hThread, 10000);
}
