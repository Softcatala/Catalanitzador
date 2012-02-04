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

#include "DownloadAction.h"
#include "DownloadInet.h"
#include "Sha1Sum.h"

int _tmain(int argc, _TCHAR* argv[])
{
	DownloadAction downloadAction;
	DownloadInet downloader;
	wstring file(L"download.bin");
	Sha1Sum sha1_computed(file), sha1_read;
	vector <Download> downloads;
	bool bFile, bSha1;
	bool bAllOk = true;

	downloads = downloadAction.GetDownloads();
	for (unsigned int i = 0; i < downloads.size(); i++)
	{
		bFile = downloader.GetFile((wchar_t*) downloads[i].download.c_str(), (wchar_t*) file.c_str(), NULL, NULL);
		bSha1 = downloadAction.GetAssociatedFileSha1Sum((DownloadID) i, file, sha1_read);
		sha1_computed.ComputeforFile();
		
		wprintf(L"* %s download file:%s, download sha1:%s, sha1 verification:%s\r\n",
			downloads[i].download.c_str(),
			bFile == true ? L"Ok" : L"Failed",
			bSha1 == true ? L"Ok" : L"Failed",
			sha1_read == sha1_computed ? L"Ok" : L"Failed");

		if (!bFile || !bSha1 || !(sha1_read == sha1_computed))
			bAllOk = false;
	}

	if (bAllOk)
		wprintf(L"%u downloads all OK!\r\n", downloads.size());
	else
		wprintf(L"Some downloads contained errors!\r\n");

	return 0;
}

