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

#define JAVA_REGKEY L"SOFTWARE\\JavaSoft\\Java Runtime Environment"

Java::Java(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
{
	m_registry = registry;
	m_OSVersion = OSVersion;
	m_runner = runner;	
}

bool Java::ShouldInstall(wstring minVersion)
{
	wstring javaStrVersion;
	bool bShouldInstallJava;

	ApplicationVersion appMinVersion(minVersion);
	javaStrVersion = GetVersion();
	bShouldInstallJava = ApplicationVersion(javaStrVersion) < appMinVersion;

	g_log.Log(L"Java::ShouldInstall. Java version %s, should install %u",
		(wchar_t*) javaStrVersion.c_str(), (wchar_t*) bShouldInstallJava);

	return bShouldInstallJava;
}

void Java::Install(wstring installer)
{
	wstring app, params;	

	app = installer;
	params = L" /s";

	g_log.Log(L"Java::Install. '%s' with params '%s'", (wchar_t*) app.c_str(), (wchar_t*) params.c_str());
	m_runner->Execute((wchar_t*) app.c_str(), (wchar_t*) params.c_str());	
}

bool Java::_readVersion(wstring& version)
{
	version.erase();
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, JAVA_REGKEY, false))
	{
		wchar_t szVersion[1024];
	
		if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
		{
			version = szVersion;
		}
		m_registry->Close();
	}

	return version.empty() == false;
}

wstring Java::GetVersion()
{
	wstring version;

	_readVersion(version);
	return version;
}

