/* 
 * Copyright (C) 2014 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
 
#pragma once


#include <vector>
using namespace std;

// This class has been used to gather information when errors ocurr
// to understand which files the user had for the application
class DirectoryListing
{
	public:

		DirectoryListing(wstring directory);
		vector <wstring> GetFilesUpToDeepth(int deepth);

	private:

		void UpdateListOfFiles(wstring directory, vector <wstring>& files);
		wstring RemoveOneSubDirectory(wstring directory);

		wstring m_directory;
};
