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

#include "DownloadInet.h"
#include "Sha1Sum.h"
#include "ConfigurationFileActionDownloads.h"
#include "ConfigurationInstance.h"

wstring bin_file(L"download.bin");
wstring sha1_file(L"download.sha1");

int _tmain(int argc, _TCHAR* argv[])
{
	vector <ConfigurationFileActionDownloads> fileActionsDownloads;
	fileActionsDownloads = ConfigurationInstance::Get().GetRemote().GetFileActionsDownloads();
	bool bAllOk = true;
	int downs_ok = 0;
	int downs_errors = 0;

	wprintf(L"Actions with configured downloads: %u\r\n", fileActionsDownloads.size());
	wprintf(L"   IDs: ");

	for (unsigned int i = 0; i < fileActionsDownloads.size(); i++)
	{
		wprintf(L"%u, ", fileActionsDownloads.at(i).GetActionID());
	}
	wprintf(L"");

	for (unsigned int i = 0; i < fileActionsDownloads.size(); i++) // Actions
	{		
		vector <ConfigurationFileActionDownload> downloads;

		downloads = fileActionsDownloads.at(i).GetFileActionDownloadCollection();

		for (unsigned int d = 0; d < downloads.size(); d++) // Download for a version
		{
			DownloadInet inetacccess;
			vector <wstring>& urls = downloads.at(d).GetUrls();
			vector <wstring>& sha1s = downloads.at(d).GetSha1Urls();		
			bool bFile, bSha1, sha1_matches;
			
			for (unsigned int x = 0; x < urls.size(); x++)
			{
				Sha1Sum sha1_computed(bin_file), sha1_read(sha1_file);
				wstring file, sha1;

				file = urls.at(x);
				sha1 = sha1s.at(x);

				bFile = inetacccess.GetFile((wchar_t *)file.c_str(), (wchar_t *)bin_file.c_str(), NULL, NULL);
				bSha1  = inetacccess.GetFile((wchar_t *)sha1.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
				sha1_computed.ComputeforFile();
				sha1_read.ReadFromFile();

				sha1_matches = sha1_read == sha1_computed;

				wprintf(L"* %s, %s download file:%s, download sha1:%s, sha1 verification:%s\r\n",
					urls.at(x).c_str(),
					sha1s.at(x).c_str(),
					bFile == true ? L"Ok" : L"Failed",
					bSha1 == true ? L"Ok" : L"Failed",
					sha1_matches ? L"Ok" : L"Failed");

				if (bFile == false || bSha1 == false || sha1_read != sha1_computed)
				{
					bAllOk = false;
					downs_errors++;
				}
				else
				{
					downs_ok++;
				}
			}
		}
	}

	if (bAllOk)
			wprintf(L"All downloads all OK!\r\n");
		else
			wprintf(L"Some downloads contained errors!\r\n");

	wprintf(L"Files Ok: %u, with errors: %u\r\n", downs_ok, downs_errors);	
}

