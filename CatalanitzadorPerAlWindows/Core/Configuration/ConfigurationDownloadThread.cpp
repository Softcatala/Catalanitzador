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
#include "ConfigurationDownloadThread.h"
#include "Sha1Sum.h"
#include "Url.h"
#include "ConfigurationInstance.h"   
#include "ConfigurationRemoteEmbedded.h"
#include "ApplicationVersion.h"
#include "ConfigurationRemoteXmlParser.h"
#include "StringConversion.h"

#define SHA1_EXTENSION L".sha1"


ConfigurationDownloadThread::~ConfigurationDownloadThread()
{	
	if (m_filename.size() > 0  && GetFileAttributes(m_filename.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename.c_str());
	}
}

wstring ConfigurationDownloadThread::_getApplicationEmbeddedConfigurationSha1()
{
	ConfigurationRemoteEmbedded configurationEmbedded;
	configurationEmbedded.Load();
	return configurationEmbedded.GetSha1Sum();
}

bool ConfigurationDownloadThread::_downloadRemoteSha1()
{
	Sha1Sum sha1sum;
	DownloadInet inetacccess;
	bool bRslt;
	wstring sha1_file(m_filename);
	wstring sha1_url;
	
	sha1_url = ConfigurationInstance::Get().GetDownloadConfigurationUrl();
	sha1_file += SHA1_EXTENSION;
	sha1_url += SHA1_EXTENSION;

	bRslt = inetacccess.GetFile((wchar_t *)sha1_url.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL);
	g_log.Log(L"ConfigurationDownloadThread::GetAssociatedFileSha1Sum '%s' is %u", (wchar_t *) sha1_url.c_str(), (wchar_t *) bRslt);

	sha1sum.SetFile(sha1_file);
	sha1sum.ReadFromFile();
	DeleteFile(sha1_file.c_str());
	m_donwloadSha1sum = sha1sum.GetSum();
	return sha1sum.GetSum().empty() == false;
}

void ConfigurationDownloadThread::_setFileName()
{
	wchar_t szFilename[MAX_PATH];
	Url url(ConfigurationInstance::Get().GetDownloadConfigurationUrl());

	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, url.GetFileName());
	m_filename = szFilename;
}

bool ConfigurationDownloadThread::_getFile(wstring surl)
{	
	DownloadInet inetacccess;
	Url url(surl);
	bool bRslt;
	
	Sha1Sum sha1_computed(m_filename);
	bRslt = inetacccess.GetFile((wchar_t *)surl.c_str(), (wchar_t*) m_filename.c_str(), NULL, NULL);
	g_log.Log(L"ConfigurationDownloadThread::_getFile '%s' is %u", (wchar_t *) surl.c_str(), (wchar_t *) bRslt);

	if (bRslt == false)
		return false;	
	
	sha1_computed.ComputeforFile();
	return sha1_computed.GetSum() == m_donwloadSha1sum;
}

void ConfigurationDownloadThread::OnStart()
{
	if (ConfigurationInstance::Get().GetDownloadConfiguration() == false)
	{
		g_log.Log(L"ConfigurationDownloadThread::OnStart. Do not download the configuration");
		return;
	}

	_setFileName();
	if (_downloadRemoteSha1() == false)
		return;

	if (_getApplicationEmbeddedConfigurationSha1() == m_donwloadSha1sum)
	{
		g_log.Log(L"ConfigurationDownloadThread::OnStart. Remote file same version that the embedded");
		return;
	}
	
	if (_getFile(ConfigurationInstance::Get().GetDownloadConfigurationUrl()) == false)
		return;

	ConfigurationRemoteXmlParser configurationXmlParser(m_filename);
	configurationXmlParser.Parse();
	
	if (_isConfigurationCompatibleWithAppVersion(configurationXmlParser.GetConfiguration()) == false)
	{
		g_log.Log(L"ConfigurationDownloadThread::OnStart. File not compatible with application");
		return;
	}

	if (configurationXmlParser.GetConfiguration().IsOlderThan(ConfigurationInstance::Get().GetRemote()))
	{
		g_log.Log(L"ConfigurationDownloadThread::OnStart. Remote file is not newer than the embedded");
		return;
	}
	
	ConfigurationInstance::Get().SetRemote(configurationXmlParser.GetConfiguration());
	g_log.Log(L"ConfigurationDownloadThread::OnStart. Using remote configuration file");
}

bool ConfigurationDownloadThread::_isConfigurationCompatibleWithAppVersion(ConfigurationRemote configuration)
{
	ApplicationVersion configurationVersion(configuration.GetCompatibility());
	return ConfigurationInstance::Get().GetVersion() >= configurationVersion;
}

