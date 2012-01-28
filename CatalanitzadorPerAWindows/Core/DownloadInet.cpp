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
#include <wchar.h>
#include "DownloadInet.h"
#include "Window.h"
#include "Url.h"
#include <stdio.h>

int DownloadInet::_getFileSize(HINTERNET hRemoteFile)
{
	const int nEstimatedSize = 20000000;
	wchar_t szSizeBuffer[64];
	DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer);
	
	if (HttpQueryInfo(hRemoteFile, HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL))
	{
		return _wtol(szSizeBuffer);
	}	
	return nEstimatedSize;
}

bool DownloadInet::GetFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
{
	HINTERNET hRemoteFile;
	HANDLE hWrite;
	DWORD dwRead, dwWritten;
	int nTotal, nCurrent;
	
	hRemoteFile = InternetOpenUrl(hInternet, URL, NULL, 0,
		INTERNET_FLAG_RELOAD, // TODO: Debug only, prevents local caching
		0);

	if (hRemoteFile == 0)
		return false;

	hWrite = CreateFile(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hWrite == INVALID_HANDLE_VALUE)
		return false;
		
	BYTE buffer[32768];

	nTotal = _getFileSize(hRemoteFile);
	nCurrent = 0;

	while (InternetReadFile(hRemoteFile, buffer, sizeof (buffer) - 1, &dwRead))
	{
		if (dwRead == 0)
			break;

		nCurrent+= dwRead;

		// In the case that the size was estimated, let's manage user expectations by increasing total by 30%
		if (nCurrent > nTotal)
			nTotal = (int) ((double) nCurrent * 1.30);
		
		if (progress != NULL)
		{
			progress (nTotal, nCurrent, data);
		}

		Window::ProcessMessages();

		WriteFile(hWrite, buffer, dwRead, &dwWritten, NULL);
	}
	CloseHandle(hWrite);
	InternetCloseHandle(hRemoteFile);
	return true;
}

