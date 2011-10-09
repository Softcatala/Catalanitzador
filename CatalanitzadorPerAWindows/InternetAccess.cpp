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
#include "InternetAccess.h"

InternetAccess::InternetAccess ()
{
	// TODO: Consider add support for proxy
	hInternet = InternetOpen (0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
}

InternetAccess::~InternetAccess ()
{
	InternetCloseHandle(hInternet);
}

bool InternetAccess::GetFile (wchar_t* URL, wchar_t* file)
{
	HINTERNET hFile;
	HANDLE hWrite;
	DWORD dwRead, dwWritten;
	
	hFile = InternetOpenUrl(hInternet, URL, NULL, 0,		
		INTERNET_FLAG_RELOAD, // TODO: Debug only, prevents local caching
		0);

	if (hFile == 0)
		return false;

	hWrite = CreateFile (file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hWrite == INVALID_HANDLE_VALUE)
		return false;  
		
	CHAR buffer[1024];
	
	while (InternetReadFile(hFile, buffer, 1023, &dwRead))
	{
		if (dwRead == 0)
			break;

		WriteFile (hWrite, buffer, dwRead, &dwWritten, NULL);
	}
	CloseHandle (hWrite);
	InternetCloseHandle(hFile);
	return true;	
}

