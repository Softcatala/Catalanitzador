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
#include "MultipleDownloads.h"

MultipleDownloads::MultipleDownloads(DownloadManager* downloadManager)
{
	m_downloadManager = downloadManager;
}

void MultipleDownloads::AddDownload(ConfigurationFileActionDownload configuration, wstring file)
{
	FileDownload fileDownload;
	fileDownload.configuration = configuration;
	fileDownload.file = file;
	m_downloads.push_back(fileDownload);
}

bool MultipleDownloads::_downloadStatus(int total, int current, void *data)
{
	MultipleDownloads* pThis = (MultipleDownloads *) data;	
	return pThis->m_temporaryProgress(pThis->m_totalBytesToDownload, pThis->m_previousDownloadedBytes + current, 
		pThis->m_temporaryData);
}

int MultipleDownloads::_getFileSizes()
{
	int totalSize = 0;
	int size;

	for (unsigned int i = 0; i < m_downloads.size(); i++)
	{
		size = m_downloadManager->GetFileSize(m_downloads[i].configuration);
		m_downloads[i].size = size;
		totalSize += size;
	}

	return totalSize;
}

bool MultipleDownloads::Download(ProgressStatus progress, void *data)
{
	m_temporaryData = data;
	m_temporaryProgress = progress;	 
	m_previousDownloadedBytes = 0;
	m_totalBytesToDownload = _getFileSizes();

	for (unsigned int i = 0; i < m_downloads.size(); i++)
	{
		if (!m_downloadManager->GetFileAndVerifyAssociatedSha1(m_downloads[i].configuration, m_downloads[i].file, 
			(ProgressStatus)&_downloadStatus, this))
		{
			return false;
		}

		m_previousDownloadedBytes += m_downloads[i].size;
	}

	return true;
}