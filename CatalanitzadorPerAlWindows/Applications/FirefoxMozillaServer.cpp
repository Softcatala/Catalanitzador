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

#define VERSION_TAG L"[version]"
#define PLATFORM_TAG L"[platform]"

FirefoxMozillaServer::FirefoxMozillaServer(DownloadManager* downloadManager, wstring version, bool is64Bits)
{
	m_downloadManager = downloadManager;
	m_version = version;
	m_is64Bits = is64Bits;
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
	wstring filename;
	wstring win32path(_getPlatformSubdir());
	int pos;
	
	_replaceString(url, L"%20", L" ");	// Poor's man URL decoder

	pos = url.find(win32path.c_str(), win32path.size());	
	filename = url.substr(pos + 1);
	return _getSha1SignatureForFilename(sha1file, filename);
}

wstring FirefoxMozillaServer::_getSha1SignatureForFilename(wstring sha1file, wstring filename)
{
	wstring line;	
	wifstream reader;
	reader.open(sha1file.c_str());

	if (!reader.is_open())	
		return false;

	while(!(getline(reader, line)).eof())
	{
		if (line.find(filename) != string::npos)
		{
			int end = 0;
			for (unsigned int i = 0; i < line.length(); i ++)
			{
				if (isalnum(line[i]))
					continue;
				
				end = i;
				break;
			}

			return line.substr(0, end);
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

wstring FirefoxMozillaServer::_getPlatformSubdir()
{
	return m_is64Bits ? L"win64" : L"win32";
}

ConfigurationFileActionDownload FirefoxMozillaServer::GetConfigurationFileActionDownload()
{
	ConfigurationFileActionDownload downloadVersion;
	wstring url;
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(FirefoxActionID, ApplicationVersion(m_version));

	if (downloadVersion.GetUrls().size() > 0)
	{
		url = downloadVersion.GetUrls().at(0);
		_replaceString(url, VERSION_TAG, m_version);
		_replaceString(url, PLATFORM_TAG, _getPlatformSubdir());
		downloadVersion.SetUrl(0, url);
	}

	return downloadVersion;
 }