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
#include "Actions.h"
#include "DownloadManager.h"

LogFile g_log;

wstring bin_file(L"download.bin");
wstring sha1_file(L"download.sha1");
wchar_t szString[2048];

void _outputString(wchar_t* string)
{
	wprintf(string);
	wprintf(L"\r\n");
	g_log.Log(string);
}

bool _downloadFile(wstring url, wstring sha1)
{
	DownloadInet inetacccess;
	Sha1Sum sha1_computed(bin_file), sha1_read(sha1_file);
	bool bFile, bSha1, sha1_matches;
		
	bFile = inetacccess.GetFile((wchar_t *)url.c_str(), (wchar_t *)bin_file.c_str(), NULL, NULL);
	bSha1 = inetacccess.GetFile((wchar_t *)sha1.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
	sha1_computed.ComputeforFile();
	sha1_read.ReadFromFile();

	sha1_matches = sha1_read == sha1_computed;

	swprintf_s(szString, L"* %s, %s download file:%s, download sha1:%s, sha1 verification:%s",
		url.c_str(),
		sha1.c_str(),
		bFile == true ? L"Ok" : L"Failed",
		bSha1 == true ? L"Ok" : L"Failed",
		sha1_matches == true ? L"Ok" : L"Failed");
	_outputString(szString);

	return bFile == true && bSha1 == true && sha1_matches == true;
}

bool _downloadConfigurationFile()
{
	wstring sha1(REMOTE_CONFIGURATION_URL);
	sha1 += L".sha1";

	return _downloadFile(REMOTE_CONFIGURATION_URL, sha1);
}

void listDefinedActions(vector <ConfigurationFileActionDownloads> fileActionsDownloads)
{
	DownloadManager downloadManager;
	Actions actions(&downloadManager);

	swprintf_s(szString, L"Actions with configured downloads: %u", fileActionsDownloads.size());
	_outputString(szString);

	for (unsigned int i = 0; i < fileActionsDownloads.size(); i++)
	{
		ActionID actionID = fileActionsDownloads.at(i).GetActionID();

		swprintf_s(szString, L"Action '%s' (id %u)", actions.GetActionFromID(actionID)->GetName(), actionID);
		_outputString(szString);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector <ConfigurationFileActionDownloads> fileActionsDownloads;
	fileActionsDownloads = ConfigurationInstance::Get().GetRemote().GetFileActionsDownloads();	
	int downs_ok = 0;
	int downs_errors = 0;
	
	g_log.CreateLog(L"DownloadFileChecker.log", L"DownloadFileChecker");
	
	if (_downloadConfigurationFile() == false)
	{
		downs_errors++;
	}	

	listDefinedActions(fileActionsDownloads);

	for (unsigned int i = 0; i < fileActionsDownloads.size(); i++) // Actions
	{		
		vector <ConfigurationFileActionDownload> downloads;

		downloads = fileActionsDownloads.at(i).GetFileActionDownloadCollection();

		for (unsigned int d = 0; d < downloads.size(); d++) // Download for a version
		{
			vector <wstring>& urls = downloads.at(d).GetUrls();
			vector <wstring>& sha1s = downloads.at(d).GetSha1Urls();
		
			for (unsigned int x = 0; x < urls.size(); x++)
			{
				if (_downloadFile(urls.at(x), sha1s.at(x)))
				{
					downs_ok++;
				}
				else
				{
					downs_errors++;
				}
			}
		}
	}

	if (downs_errors == 0)
			_outputString(L"All downloads all OK!");
		else
			_outputString(L"Some downloads contained errors!");

	swprintf_s(szString, L"Files Ok: %u, with errors: %u", downs_ok, downs_errors);
	_outputString(szString);
}

