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
#include "InternetAccess.h"
#include "Window.h"
#include "Url.h"
#include <stdio.h>

InternetAccess::InternetAccess()
{
	hInternet = InternetOpen(0, INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
}

InternetAccess::~InternetAccess()
{
	InternetCloseHandle(hInternet);
}

int InternetAccess::_getFileSize(HINTERNET hRemoteFile)
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

bool InternetAccess::GetFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
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

// Encodes a single variable form
void InternetAccess::_urlFormEncode(char* variables, char* encoded)
{	
	int len;
	char* pos = variables;
	bool bEqualFound = false;

	encoded [0] = 0;
	len = strlen(variables);	

	for (int i = 0; i < len; i++, pos++)
	{		
		if (bEqualFound == false // Until variable = assignation is not found do not start encoding
			|| *pos =='.' ||
			(*pos >='0' && *pos <='9') ||
			(*pos >='A' && *pos <='Z') ||
			(*pos >='a' && *pos <='z'))
		{
			char ch[2];

			ch[0] = *pos;
			ch[1] = NULL;
			strcat (encoded, ch);
		}
		else if (*pos == '\r' || *pos == '\n' || *pos == '\t')
		{
			// Skip cr, lf, tab
		}
		else if (*pos == ' ')
		{
			char ch[2];

			ch[0] = '+';
			ch[1] = NULL;
			strcat (encoded, ch);
		}
		else
		{
			char string [16];
			sprintf (string, "%%%X", *pos);
			strcat (encoded, string);
		}

		if (bEqualFound == false && *pos == '=')
		{
			bEqualFound = true;
		}
	}
}

// Variables are added in ANSI at the end of the request that's why is an ANSI string
bool InternetAccess::PostForm(wchar_t* URL, char* variables)
{
	HINTERNET hConnect, hRequest;
	const wchar_t hdrs[] = L"Content-Type: application/x-www-form-urlencoded\n\r";
	const wchar_t* accept[2]= {L"*/*", NULL};
	Url url (URL);
	
	hConnect = InternetConnect(hInternet, url.GetHostname(),
		INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);

	if (hConnect == NULL)	
		return false;	

	hRequest = HttpOpenRequest(hConnect, L"POST", url.GetPathAndFileName(),  NULL, NULL, accept,
	  INTERNET_FLAG_RELOAD |INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT, 0);

	if (hRequest == NULL)
	{
		InternetCloseHandle(hConnect);
		return false;
	}
	
	char szEncoded[65535];	
	_urlFormEncode(variables, szEncoded);

	HttpSendRequest(hRequest, hdrs, wcslen(hdrs), szEncoded, strlen(szEncoded));	

	// Get result of the operation
	DWORD dwCode = 0;
	DWORD dwSize = sizeof(dwCode);
	HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | 
			HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL);
	
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	return dwCode == 200;
}

bool InternetAccess::IsThereConnection()
{
    DWORD dwConnectionFlags = 0;

    if (!InternetGetConnectedState(&dwConnectionFlags, 0))
        return false;

    if (InternetAttemptConnect(0) != ERROR_SUCCESS)
        return false;

    return true;
}
