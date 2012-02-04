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
 
DownloadAction::DownloadAction()
{
	m_downloads.resize(DI_LENGTH - 1);

	m_downloads[DI_MSOFFICEACTION_2010] = Download(wstring(MSOFFICEACTION_2010), 5199);
	m_downloads[DI_MSOFFICEACTION_2007] = Download(wstring(MSOFFICEACTION_2007), 3481);
	m_downloads[DI_MSOFFICEACTION_2003] = Download(wstring(MSOFFICEACTION_2003), 3444);
	m_downloads[DI_WINDOWSLPIACTION_XP] = Download(wstring(WINDOWSLPIACTION_XP), 3468);
	m_downloads[DI_WINDOWSLPIACTION_XP_SP2] = Download(wstring(WINDOWSLPIACTION_XP_SP2), 3468);
	m_downloads[DI_WINDOWSLPIACTION_VISTA] = Download(wstring(WINDOWSLPIACTION_VISTA), 3511);
	m_downloads[DI_WINDOWSLPIACTION_7] = Download(wstring(WINDOWSLPIACTION_7), 4932);
	m_downloads[DI_WINDOWSLPIACTION_7_64BITS] = Download(wstring(WINDOWSLPIACTION_7_64BITS), 4932);
	m_downloads[DI_IELPI_IE7] = Download(wstring(IELPI_IE7), 4756);
	m_downloads[DI_IELPI_IE8_XP] = Download(wstring(IELPI_IE8_XP), 4755);
	m_downloads[DI_IELPI_IE8_VISTA] = Download(wstring(IELPI_IE8_VISTA), 5721);
	m_downloads[DI_IELPI_IE9_VISTA] = Download(wstring(IELPI_IE9_VISTA), 5721);
	m_downloads[DI_IELPI_IE9_7] = Download(wstring(IELPI_IE9_7), 5722);
	m_downloads[DI_IELPI_IE9_7_64BITS] = Download(wstring(IELPI_IE9_7_64BITS), 5722);
}

wstring DownloadAction::GetFileName(DownloadID downloadID)
{
	return m_downloads[downloadID].download;
}

bool DownloadAction::GetAssociatedFileSha1Sum(DownloadID downloadID, wstring sha1_file, Sha1Sum &sha1sum)
{
	wstring sha1_url;
	DownloadInet inetacccess;
	bool bRslt;

	sha1_url = GetFileName(downloadID);
	sha1_file += SHA1_EXTESION;
	sha1_url += SHA1_EXTESION;

	bRslt = inetacccess.GetFile((wchar_t *)sha1_url.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
	g_log.Log(L"DownloadAction::GetAssociatedFileSha1Sum '%s' is %u", (wchar_t *) sha1_url.c_str(), (wchar_t *) bRslt);

	sha1sum.SetFile(sha1_file);
	sha1sum.ReadFromFile();
	DeleteFile(sha1_file.c_str());
	return sha1sum.GetSum().empty() == false;
}

void DownloadAction::_getRebost(DownloadID downloadID, wstring &url)
{
	wchar_t szURL[2048];	
	
	swprintf_s(szURL, L"http://baixades.softcatala.org/?url=%s&id=%u&mirall=catalanitzador&so=win32&versio=1.0",
		m_downloads[downloadID].download.c_str(),
		m_downloads[downloadID].id);

	url = szURL;
}

bool DownloadAction::GetFile(DownloadID downloadID, wstring file, ProgressStatus progress, void *data)
{	
	DownloadInet inetacccess;
	Sha1Sum sha1_computed(file), sha1_read;
	wstring url;
	bool bRslt;	
	
	_getRebost(downloadID, url);
	bRslt = inetacccess.GetFile((wchar_t *)url.c_str(), (wchar_t *)file.c_str(), progress, data);
	g_log.Log(L"DownloadAction::GetFile '%s' is %u", (wchar_t *) url.c_str(), (wchar_t *) bRslt);

	if (bRslt == false)
		return false;	

	// If cannot get the file, cannot check assume that is OK
	if (GetAssociatedFileSha1Sum(downloadID, (wchar_t *)file.c_str(), sha1_read) == false)
		return true;
	
	sha1_computed.ComputeforFile();
	return sha1_computed == sha1_read;
}