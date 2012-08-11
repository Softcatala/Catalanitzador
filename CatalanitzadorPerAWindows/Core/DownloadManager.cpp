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
#include "DownloadManager.h"
#define SHA1_EXTENSION L".sha1"


bool DownloadManager::_getAssociatedFileSha1Sum(wstring sha1_url, wstring sha1_file, Sha1Sum &sha1sum)
{	
	DownloadInet inetacccess;
	bool bRslt;

	sha1_file += SHA1_EXTENSION;
	sha1_url += SHA1_EXTENSION;

	bRslt = inetacccess.GetFile((wchar_t *)sha1_url.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
	g_log.Log(L"DownloadManager::GetAssociatedFileSha1Sum '%s' is %u", (wchar_t *) sha1_url.c_str(), (wchar_t *) bRslt);

	sha1sum.SetFile(sha1_file);
	sha1sum.ReadFromFile();
	DeleteFile(sha1_file.c_str());
	return sha1sum.GetSum().empty() == false;
}

bool DownloadManager::GetFile(ConfigurationFileActionDownload configuration, wstring file, ProgressStatus progress, void *data)
{
	DownloadInet inetacccess;
	Sha1Sum sha1_computed(file), sha1_read;
	wstring url;
	bool bRslt;

	for (unsigned index = 0; index < configuration.GetUrls().size(); index++)
	{
		url = configuration.GetUrls().at(index);
		bRslt = inetacccess.GetFile((wchar_t *)url.c_str(), (wchar_t *)file.c_str(), progress, data);
		g_log.Log(L"DownloadManager::GetFile '%s' is %u", (wchar_t *) url.c_str(), (wchar_t *) bRslt);

		if (bRslt == false)
			continue;

		// If cannot get the sha1 file, we cannot verify the download and report it as incorrect
		if (_getAssociatedFileSha1Sum(url, (wchar_t *)file.c_str(), sha1_read) == false)
			continue;
		
		sha1_computed.ComputeforFile();
		if (sha1_computed == sha1_read)
			return true;
	}
	return false;
}
