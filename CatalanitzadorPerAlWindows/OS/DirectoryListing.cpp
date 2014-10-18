/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Shlwapi.h"
#include "DirectoryListing.h"

DirectoryListing::DirectoryListing(wstring directory)
{
	m_directory = directory;
}

vector <wstring> DirectoryListing::GetFilesUpToDeepth(int deepth)
{
	vector <wstring> files;
	wstring directory = m_directory;

	for (int i = 0; i < deepth; i++)
	{
		UpdateListOfFiles(directory, files);
		directory = RemoveOneSubDirectory(directory);
	}

	return files;
}

wstring DirectoryListing::RemoveOneSubDirectory(wstring directory)
{
	if (directory.size() == 0)
		return directory;

	// Path ends with /
	int pos = directory.find_last_of(L"/\\");
	if (pos == directory.size() - 1)
	{
		directory = directory.substr(0, directory.size() -1);
		pos = directory.find_last_of(L"/\\");
	}

	// Remove subdirectory
	if (pos > 0)
	{
		directory = directory.substr(0, pos + 1);
	}
	
	return directory;	
}

void DirectoryListing::UpdateListOfFiles(wstring directory, vector <wstring>& files)
{	
	wchar_t search_path[MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	wsprintf(search_path, L"%s*.*", directory.c_str());
	hFind = FindFirstFile(search_path, &fd); 

	if (hFind == INVALID_HANDLE_VALUE)
		return;
	
	do 
	{ 
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			files.push_back(directory + fd.cFileName);
		}

	}while(FindNextFile(hFind, &fd)); 
	
	::FindClose(hFind);
}