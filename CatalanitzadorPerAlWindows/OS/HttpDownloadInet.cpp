/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "HttpDownloadInet.h"
#include "Window.h"
#include "Url.h"

#define ERROR_SERVER_ERROR 500
#define ERROR_FILE_NOTFOUND 404

HINTERNET HttpDownloadInet::_internetOpenUrl(wchar_t* URL) const
{
	HINTERNET hRemoteFile;

	hRemoteFile = InternetOpenUrl(m_hInternet, URL, NULL, 0,
#if DEVELOPMENT_VERSION
		0, // Allows catching (speeds ups downloads during development)
#else
		INTERNET_FLAG_RELOAD, // Prevents local caching (for release version)
#endif
		0);	

	if (hRemoteFile == 0)
		return NULL;

	int status = _getStatusCode(hRemoteFile);
	if (status == ERROR_FILE_NOTFOUND || status == ERROR_SERVER_ERROR)
	{
		g_log.Log(L"HttpDownloadInet::_internetOpenUrl. Error '%u' getting '%s'", (wchar_t *) status, URL);
		InternetCloseHandle(hRemoteFile);
		return NULL;
	}
	return hRemoteFile;
}

int HttpDownloadInet::GetFileSize(wchar_t* URL) const
{
	int nTotal;	
	HINTERNET hRemoteFile;

	hRemoteFile = _internetOpenUrl(URL);

	if (hRemoteFile == NULL)
		return 0;	
	
	nTotal = _getFileSize(hRemoteFile);
	InternetCloseHandle(hRemoteFile);
	return nTotal;
}

int HttpDownloadInet::_getFileSize(HINTERNET hRemoteFile) const
{
	wchar_t szSizeBuffer[64];
	DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer);
	
	if (HttpQueryInfo(hRemoteFile, HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL))
	{
		return _wtol(szSizeBuffer);
	}	
	return 0;
}

int HttpDownloadInet::_getStatusCode(HINTERNET hRemoteFile) const
{	
	DWORD dwStatus;
	DWORD dwStatusSize = sizeof(dwStatus);
	
	if (HttpQueryInfo(hRemoteFile, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwStatusSize, NULL))
	{
		return dwStatus;
	}	
	return ERROR_FILE_NOTFOUND;
}

bool HttpDownloadInet::GetFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
{	
	HINTERNET hRemoteFile;
	HANDLE hWrite;
	DWORD dwRead, dwWritten;
	int nTotal, nCurrent;	
	Url url(URL);

	hRemoteFile = _internetOpenUrl(URL);

	if (hRemoteFile == NULL)
		return false;
	
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

	CloseHandle(hWrite);
	InternetCloseHandle(hRemoteFile);
	return dwRead == 0;
}

