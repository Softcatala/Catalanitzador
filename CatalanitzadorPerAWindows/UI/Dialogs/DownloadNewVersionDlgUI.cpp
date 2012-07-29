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

#include <stdafx.h>
#include "DownloadNewVersionDlgUI.h"
#include "DownloadInet.h"
#include "ConfigurationInstance.h"
#include <CommCtrl.h>
#include "Runner.h"
#include "Url.h"

#define TIMER_ID 912
#define FILENAME L"CatalanitzadorPerAlWindows.exe"
#define PROGRAM_NAME L"Catalanitzador per al Windows"

const float BYTES_TO_MEGABYTES = 1024*1024;

void DownloadNewVersionDlgUI::_onInitDialog()
{
	m_bCancelled = FALSE;
	m_hProgressBar = GetDlgItem (getHandle(), IDC_UPDATEAPPLICATION_PROGRESSBAR);
	m_hDescription = GetDlgItem (getHandle(), IDC_UPDATEAPPLICATION_DESCRIPTION);
	SetTimer(getHandle(), TIMER_ID, 500, NULL);
}

void DownloadNewVersionDlgUI::OnDownloadStatus(int total, int current)
{	
	wchar_t szString[MAX_LOADSTRING];
	wchar_t szText[MAX_LOADSTRING];

	SendMessage(m_hProgressBar, PBM_SETRANGE32, 0, total);
	SendMessage(m_hProgressBar, PBM_SETPOS, current, 0);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_DOWNLOAD, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, PROGRAM_NAME, ((float)current) / BYTES_TO_MEGABYTES, ((float)total) / BYTES_TO_MEGABYTES);
	SendMessage(m_hDescription, WM_SETTEXT, 0, (LPARAM) szText);
}

void DownloadNewVersionDlgUI::_downloadStatus(int total, int current, void *data)
{
	DownloadNewVersionDlgUI* pThis = (DownloadNewVersionDlgUI *) data;
	pThis->OnDownloadStatus(total, current);	
}

void DownloadNewVersionDlgUI::_downloadFile()
{
	DownloadInet downloadInet;
	Configuration configuration = ConfigurationInstance::Get();
	wstring surl;

	// TODO: Implement re-try for other mirrors
	surl = configuration.GetRemote().GetLatest().GetUrls().at(0);

	wchar_t szFilename[MAX_PATH];
	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, FILENAME);
	m_filename = szFilename;

	downloadInet.GetFile((wchar_t *)surl.c_str(), (wchar_t *)m_filename.c_str(), _downloadStatus, this);
}

void DownloadNewVersionDlgUI::_onTimer()
{
	KillTimer(getHandle(), TIMER_ID);	
	_downloadFile();

	if (m_bCancelled == FALSE)
	{
		Runner runner;
		runner.Execute((wchar_t *)m_filename.c_str(), L"/NoRunningCheck", false);
		PostMessage(getHandle(), WM_QUIT, 0, 0);
	}
}

void DownloadNewVersionDlgUI::_onCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDCANCEL)
	{
		m_bCancelled = TRUE;
	}
}