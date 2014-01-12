/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FtpDownloadInet.h"
#include "Window.h"
#include "Url.h"

#define DEFAULT_FTP_USERNAME L"anonymous"
#define DEFAULT_FTP_PASSWORD L"anonymous@catalanitzador"

int FtpDownloadInet::GetFileSize(wchar_t* URL) const
{
	HINTERNET hFtp = NULL;
	HINTERNET hRemoteFile;	
	int nTotal;
	Url url(URL);
 
	hFtp = InternetConnect(m_hInternet, url.GetHostname(),
		0, DEFAULT_FTP_USERNAME, DEFAULT_FTP_PASSWORD, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE,0);

	if (hFtp == 0)
	{
		DWORD status = GetLastError();

		g_log.Log(L"FtpDownloadInet::GetFileSize. Error '%u' calling InternetConnect and getting '%s'", (wchar_t *) status, URL);
		return false;
	}

	hRemoteFile = FtpOpenFile(hFtp, url.GetPathAndFileName(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY,0);
	
	if (hRemoteFile == 0)
	{
		DWORD status = GetLastError();

		g_log.Log(L"FtpDownloadInet::GetFileSize. Error '%u' calling FtpOpenFile and getting '%s'", (wchar_t *) status, URL);
		return false;
	}

	nTotal = _getFileSize(hRemoteFile);

	InternetCloseHandle(hFtp);
	InternetCloseHandle(hRemoteFile);
	return nTotal;
}

int FtpDownloadInet::_getFileSize(HINTERNET hRemoteFile) const
{	
	DWORD lpdwFileSizeHigh, lpdwFileSizeLow;
	lpdwFileSizeLow = FtpGetFileSize(hRemoteFile, &lpdwFileSizeHigh);
	return lpdwFileSizeLow;
}

bool FtpDownloadInet::GetFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
{
	HINTERNET hFtp = NULL;
	HINTERNET hRemoteFile;
	HANDLE hWrite;
	DWORD dwRead, dwWritten;
	int nTotal, nCurrent;	
	Url url(URL);
 
	hFtp = InternetConnect(m_hInternet, url.GetHostname(),
		0, DEFAULT_FTP_USERNAME, DEFAULT_FTP_PASSWORD, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE,0);

	if (hFtp == 0)
	{
		DWORD status = GetLastError();

		g_log.Log(L"FtpDownloadInet::GetFile. Error '%u' calling InternetConnect and getting '%s'", (wchar_t *) status, URL);
		return false;
	}

	hRemoteFile = FtpOpenFile(hFtp, url.GetPathAndFileName(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY,0);
	
	if (hRemoteFile == 0)
	{
		DWORD status = GetLastError();

		g_log.Log(L"FtpDownloadInet::GetFile. Error '%u' calling FtpOpenFile and getting '%s'", (wchar_t *) status, URL);
		return false;
	}

	hWrite = CreateFile(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hWrite == INVALID_HANDLE_VALUE)
	{
		InternetCloseHandle(hRemoteFile);
		return false;
	}
		
	BYTE buffer[32768];

	nTotal = _getFileSize(hRemoteFile);	
	nCurrent = 0;

	while (InternetReadFile(hRemoteFile, buffer, sizeof (buffer) - 1, &dwRead))
	{
		if (dwRead == 0)
			break;

		nCurrent+= dwRead;

		if (progress != NULL)
		{
			if (progress (nTotal, nCurrent, data) == false)
				break;
		}

		Window::ProcessMessages();

		WriteFile(hWrite, buffer, dwRead, &dwWritten, NULL);
	}
	
	InternetCloseHandle(hFtp);
	CloseHandle(hWrite);
	InternetCloseHandle(hRemoteFile);
	return dwRead == 0;
}

