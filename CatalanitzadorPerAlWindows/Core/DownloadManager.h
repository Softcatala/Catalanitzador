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
 
#pragma once

#include "ConfigurationFileActionDownloads.h"
#include "ApplicationVersion.h"
#include "DownloadInet.h"
#include "Sha1Sum.h"

#include <string>
using namespace std;

class DownloadManager
{
	public:		
		
		bool GetFileAndVerifyAssociatedSha1(ConfigurationFileActionDownload configuration, wstring file, ProgressStatus progress, void *data);
		bool GetFileAndVerifyAssociatedSha1(ConfigurationFileActionDownload configuration, wstring file, Sha1Sum sha1sum, ProgressStatus progress, void *data);
		int GetFileSize(ConfigurationFileActionDownload configuration) const;
		
	private:

		bool _getAssociatedFileSha1Sum(wstring sha1_url, wstring sha1_file, Sha1Sum &sha1sum);

		vector <ConfigurationFileActionDownloads> m_fileActionsDownloads;
};
