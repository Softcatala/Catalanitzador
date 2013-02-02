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
#include "FirefoxMozillaServer.h"
#include "ConfigurationInstance.h"

#define SHA1LEN 40
#define VERSION_TAG L"[version]"
#define URL_WIN32PATH L"/win32"

FirefoxMozillaServer::FirefoxMozillaServer(DownloadManager* downloadManager, wstring version)
{
	m_downloadManager = downloadManager;
	m_version = version;
}

void FirefoxMozillaServer::_replaceString(wstring& string, wstring search, wstring replace)
{	
	size_t index = 0;

	while(true) 
	{		 
		 index = string.find(search);

		 if (index == string::npos) 
			 break;	 
		 
		 string.replace(index, search.size(), replace);
	}
}

wstring FirefoxMozillaServer::_readSha1FileSignature(wstring url, wstring sha1file)
{
	wstring line, search;
	wifstream reader;
	wstring win32path(URL_WIN32PATH);
	int pos;
	
	_replaceString(url, L"%20", L" ");	// Poor's man URL decoder

	pos = url.find(win32path.c_str(), win32path.size());	
	search = url.substr(pos + 1);

	reader.open(sha1file.c_str());

	if (!reader.is_open())	
		return false;

	while(!(getline(reader, line)).eof())
	{
		if (line.find(search) != string::npos)
		{		
			return line.substr(0, SHA1LEN);
		}
	}

	return wstring();
}

wstring FirefoxMozillaServer::GetSha1FileSignature(ConfigurationFileActionDownload downloadVersion)
{
	wstring sha1_signature, sha1url;
	DownloadInet inetacccess;
	wchar_t m_szTempFile[MAX_PATH];
	wchar_t m_szTempPath[MAX_PATH];
	bool bRslt;

	sha1url = downloadVersion.GetSha1Urls().at(0);
	_replaceString(sha1url, VERSION_TAG, m_version);

	GetTempPath(MAX_PATH, m_szTempPath);
	GetTempFileName(m_szTempPath, L"CAT", 0, m_szTempFile);
	
	bRslt = inetacccess.GetFile((wchar_t *)sha1url.c_str(), m_szTempFile, NULL, NULL);
	sha1_signature = _readSha1FileSignature(downloadVersion.GetUrls().at(0), m_szTempFile);
	DeleteFile(m_szTempFile);
	return sha1_signature;
}

ConfigurationFileActionDownload FirefoxMozillaServer::GetConfigurationFileActionDownload()
{
	ConfigurationFileActionDownload downloadVersion;
	wstring url, sha1;
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(Firefox, ApplicationVersion(m_version));

	url = downloadVersion.GetUrls().at(0);
	_replaceString(url, VERSION_TAG, m_version);
	downloadVersion.SetUrl(0, url);

	return downloadVersion;
 }