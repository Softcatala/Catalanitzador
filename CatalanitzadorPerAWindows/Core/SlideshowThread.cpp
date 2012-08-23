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

#define SlideshowThread_RSCTYPE L"SlideshowThread_FILES"
#define INDEX_FILE L"index.html"

SlideshowThread::SlideshowThread()
{
	_createURL();
}

SlideshowThread::~SlideshowThread()
{
	for (unsigned int i = 0; i < m_tempFiles.size(); i++)
	{
		DeleteFile(m_tempFiles[i].c_str());
	}
}

void SlideshowThread::_createURL()
{
	wchar_t tempPath[MAX_PATH], url[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);

	wsprintf(url, L"file:///%s%s", tempPath, INDEX_FILE);
	m_URL = url;
}

void SlideshowThread::OnStart()
{
	vector <wstring> resources;
	wchar_t tempPath[MAX_PATH];
	wstring file;

	GetTempPath(MAX_PATH, tempPath);
	Resources::EnumResources(SlideshowThread_RSCTYPE, resources);

	for (unsigned i = 0; i < resources.size(); i++)
	{
		file = tempPath;
		file += resources[i].substr(1, resources[i].size() -2);	
		Resources::DumpResource(SlideshowThread_RSCTYPE, resources[i].c_str(), (wchar_t *)file.c_str());
		m_tempFiles.push_back(wstring(file));
	}
}
