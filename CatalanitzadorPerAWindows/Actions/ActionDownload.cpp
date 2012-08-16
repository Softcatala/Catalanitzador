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
#include "Action.h"
 
ActionDownload::ActionDownload()
{
	m_downloads.resize(DI_LENGTH - 1);

	m_downloads[DI_ADOBEREADER_95] = Download(wstring(ADOBEREADER_95), 4712);
	m_downloads[DI_ADOBEREADER_1010] = Download(wstring(ADOBEREADER_1010), 4712);
	m_downloads[DI_MSLIVE2009] = Download(wstring(MSLIVE2009), 4442);
}

wstring ActionDownload::GetFileName(DownloadID downloadID)
{
	return m_downloads[downloadID].download;
}

bool ActionDownload::GetAssociatedFileSha1Sum(DownloadID downloadID, wstring sha1_file, Sha1Sum &sha1sum)
{
	wstring sha1_url;
	DownloadInet inetacccess;
	bool bRslt;

	sha1_url = GetFileName(downloadID);
	sha1_file += SHA1_EXTENSION;
	sha1_url += SHA1_EXTENSION;

	bRslt = inetacccess.GetFile((wchar_t *)sha1_url.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
	g_log.Log(L"ActionDownload::GetAssociatedFileSha1Sum '%s' is %u", (wchar_t *) sha1_url.c_str(), (wchar_t *) bRslt);

	sha1sum.SetFile(sha1_file);
	sha1sum.ReadFromFile();
	DeleteFile(sha1_file.c_str());
	return sha1sum.GetSum().empty() == false;
}

void ActionDownload::_getRebost(DownloadID downloadID, wstring &url)
{
	wchar_t szURL[2048];

	if (m_downloads[downloadID].id > 0)
	{
		swprintf_s(szURL, L"http://baixades.softcatala.org/?url=%s&id=%u&mirall=catalanitzador&so=win32&versio=1.0",
			m_downloads[downloadID].download.c_str(),
			m_downloads[downloadID].id);

		url = szURL;
	}
	else
	{
		url = m_downloads[downloadID].download.c_str();
	}
}

bool ActionDownload::GetFile(DownloadID downloadID, wstring file, ProgressStatus progress, void *data)
{	
	DownloadInet inetacccess;
	Sha1Sum sha1_computed(file), sha1_read;
	wstring url;
	bool bRslt;
	
	_getRebost(downloadID, url);
	bRslt = inetacccess.GetFile((wchar_t *)url.c_str(), (wchar_t *)file.c_str(), progress, data);
	g_log.Log(L"ActionDownload::GetFile '%s' is %u", (wchar_t *) url.c_str(), (wchar_t *) bRslt);

	if (bRslt == false)
		return false;	

	// If cannot get the sh1 file, we cannot verify the download and report it as incorrect
	if (GetAssociatedFileSha1Sum(downloadID, (wchar_t *)file.c_str(), sha1_read) == false)
		return false;
	
	sha1_computed.ComputeforFile();
	return sha1_computed == sha1_read;
}