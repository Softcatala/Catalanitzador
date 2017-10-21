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
#include "Firefox.h"
#include "OSVersion.h"

#define FIREFOX_REGKEY L"SOFTWARE\\Mozilla\\Mozilla Firefox"

Firefox::Firefox(IRegistry* registry, IOSVersion* OSVersion)
{	
	m_registry = registry;
	m_cachedValues = false;
	m_OSVersion = OSVersion;
	m_is64bits = false;
}

wstring Firefox::GetVersion()
{
	_readVersionAndLocale();
	return m_version;
}

wstring Firefox::GetLocale()
{
	_readVersionAndLocale();
	return m_locale;
}

wstring Firefox::GetInstallPath()
{
	_readInstallPath();
	return m_installPath;
}

void Firefox::_readVersionAndLocale()
{
	if (m_cachedValues)
		return;

	wstring versionWithLocale;

	versionWithLocale = _getVersionAndLocaleFromRegistry();
	_extractLocaleAndVersion(versionWithLocale);
	m_cachedValues = true;
}

void Firefox::_extractLocaleAndVersion(wstring versionWithLocale)
{	
	int pos, start, end;

	m_version.empty();
	m_locale.empty();
	start = versionWithLocale.find(L" ");

	if (start == wstring::npos)
		return;
	
	m_version = versionWithLocale.substr(0, start);			

	end = versionWithLocale.rfind(L")");

	if (end != wstring::npos)
	{
		for (pos = end  - 1; pos > start; pos--)
		{
			if (versionWithLocale[pos] == L' ' || versionWithLocale[pos] == L'(')
			{
				pos++;
				m_locale = versionWithLocale.substr(pos, end - pos);
				break;
			}
		}
	}	
}

wstring Firefox::_getVersionAndLocaleFromRegistry()
{
	wstring version;	

	if (m_OSVersion->IsWindows64Bits() && m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, FIREFOX_REGKEY, false))
	{
		m_is64bits = true;
	}
	else if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, FIREFOX_REGKEY, false))
	{
		m_is64bits = false;
	}
	else
	{
		g_log.Log(L"Firefox::_getVersionAndLocaleFromRegistry. Cannot open registry key");
		return version;
	}
	
	wchar_t szVersion[1024];
	
	if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
	{
		g_log.Log(L"Firefox::_getVersionAndLocaleFromRegistry. Firefox version %s", (wchar_t*) szVersion);
		version = szVersion;
	}

	m_registry->Close();
	return version;
}

void Firefox::_readInstallPath()
{
	// Installation path should not change from execution to execution
	if (m_installPath.empty() == false)
		return;

	wstring version;
	version = _getVersionAndLocaleFromRegistry();
	
	m_installPath.empty();
	wstring key(FIREFOX_REGKEY);
	key += L"\\" + version + L"\\Main";

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false) == false)
	{
		g_log.Log(L"Firefox::_readInstallPath. Cannot open registry key");
		return;
	}
	
	wchar_t szPath[MAX_PATH];	
	if (m_registry->GetString(L"Install Directory", szPath, sizeof(szPath)))
	{
		m_installPath = szPath;
	}
	m_registry->Close();
}

wstring Firefox::GetUserDataDirectory()
{
	wstring location;
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Mozilla\\";
	return location;
}

wstring Firefox::GetProfileRootDir()
{
	wstring location;
	
	location = GetUserDataDirectory();
	location += L"Firefox\\";
	return location;
}
