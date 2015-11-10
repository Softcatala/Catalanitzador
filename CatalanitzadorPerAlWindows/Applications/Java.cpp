/* 
 * Copyright (C) 2015 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Java.h"
#include "ApplicationVersion.h"
#include "ConfigurationInstance.h"

#define JAVA_REGKEY L"SOFTWARE\\JavaSoft\\Java Runtime Environment"
#define JAVA_CONFIGURTION_XML_ACTION_ID LangToolLibreOfficeActionID

Java::Java(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
{
	m_registry = registry;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_szFilenameJava[0] = NULL;
	m_is64bits = false;
}

Java::~Java()
{
	if (m_szFilenameJava[0] != NULL  && GetFileAttributes(m_szFilenameJava) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilenameJava);
	}
}

bool Java::ShouldInstall(wstring minVersion)
{
	wstring javaStrVersion;
	bool bShouldInstallJava;

	ApplicationVersion appMinVersion(minVersion);	
	bool is64Bits = _readVersion(javaStrVersion);
	bShouldInstallJava = (is64Bits != m_is64bits || ApplicationVersion(javaStrVersion) < appMinVersion);

	g_log.Log(L"Java::ShouldInstall. Java version '%s' 64bits %u, should install %u",
		(wchar_t*) javaStrVersion.c_str(), (wchar_t*) m_is64bits, (wchar_t*) bShouldInstallJava);

	return bShouldInstallJava;
}

void Java::Install()
{
	wstring app, params;	

	app = m_szFilenameJava;
	params = L" /s";

	m_runner->Execute((wchar_t*) app.c_str(), (wchar_t*) params.c_str(), m_is64bits);
	g_log.Log(L"Java::Install. '%s' with params '%s' 64 bits '%u'", (wchar_t*) app.c_str(), (wchar_t*) params.c_str(), (wchar_t*) m_is64bits);
}

bool Java::_readVersion(wstring& version)
{
	version.erase();
	bool is64Bits = false;
	bool rslt = false;

	if (m_is64bits && m_OSVersion->IsWindows64Bits())
	{
		rslt = m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, JAVA_REGKEY, false);
		is64Bits = rslt;
	}

	if (rslt == false)
	{
		rslt = m_registry->OpenKey(HKEY_LOCAL_MACHINE, JAVA_REGKEY, false);
	}

	if (rslt)
	{
		wchar_t szVersion[1024];
	
		if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
		{
			version = szVersion;
		}
		m_registry->Close();
	}

	return is64Bits;
}

wstring Java::GetVersion()
{
	wstring version;

	_readVersion(version);
	return version;
}

void Java::AddDownload(MultipleDownloads& multipleDownloads)
{
	ConfigurationFileActionDownload downloadVersion;

	wstring downloadID = m_is64bits ? L"Java64bits" : L"Java";
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(JAVA_CONFIGURTION_XML_ACTION_ID, downloadID);
	GetTempPath(MAX_PATH, m_szFilenameJava);
	wcscat_s(m_szFilenameJava, downloadVersion.GetFilename().c_str());
	multipleDownloads.AddDownload(downloadVersion, m_szFilenameJava);
}


