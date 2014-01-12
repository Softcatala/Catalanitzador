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
#include "DownloadInet.h"
#include "FtpDownloadInet.h"
#include "HttpDownloadInet.h"
#include "Url.h"

#define HTTP_SCHEME L"http"
#define FTP_SCHEME L"ftp"


int DownloadInet::GetFileSize(wchar_t* URL) const
{
	Url url(URL);

	if (wcscmp(url.GetScheme(), HTTP_SCHEME) == 0)
	{
		HttpDownloadInet httpDownloadInet;
		return httpDownloadInet.GetFileSize(URL);
	}

	if (wcscmp(url.GetScheme(), FTP_SCHEME) == 0)
	{
		FtpDownloadInet ftpDownloadInet;
		return ftpDownloadInet.GetFileSize(URL);
	}

	assert(true);
	return 0;
}

bool DownloadInet::GetFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
{
	Url url(URL);

	if (wcscmp(url.GetScheme(), HTTP_SCHEME) == 0)
	{
		HttpDownloadInet httpDownloadInet;
		return httpDownloadInet.GetFile(URL, file, progress, data);
	}

	if (wcscmp(url.GetScheme(), FTP_SCHEME) == 0)
	{
		FtpDownloadInet ftpDownloadInet;
		return ftpDownloadInet.GetFile(URL, file, progress, data);
	}

	assert(true);
	return false;
}
