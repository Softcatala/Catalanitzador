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
 
#pragma once

#include "ConfigurationFileActionDownloads.h"
#include "IDownloadInet.h"
#include "DownloadManager.h"

#include <string>
#include <vector>
using namespace std;

// Manages a list of files to download and offers an external Download method
// that follows the same API signature that methods supporting one single file
// removing the caller for managing the complexity of multiple downloads
class MultipleDownloads
{
	public:
		MultipleDownloads(DownloadManager* downloadManager);

		void EmptyList();
		void SetProgress(ProgressStatus progress, void *data);
		void AddDownload(ConfigurationFileActionDownload configuration, wstring file);
		bool Download(ProgressStatus progress, void *data);		

	protected:

		struct FileDownload
		{
			ConfigurationFileActionDownload configuration;
			wstring file;
			int size;
		};

		vector <FileDownload> m_downloads;

	private:

		static bool _downloadStatus(int total, int current, void *data);
		int _getFileSizes();

		DownloadManager* m_downloadManager;
		void * m_temporaryData;
		ProgressStatus m_temporaryProgress;
		int m_totalBytesToDownload;
		int m_previousDownloadedBytes;
};
