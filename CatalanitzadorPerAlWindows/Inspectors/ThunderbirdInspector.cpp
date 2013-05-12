/*
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
 * Copyright (C) 2012 Joan Montané <joan@montane.cat>
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
#include "ThunderbirdInspector.h"
#include "FileVersionInfo.h"

ThunderbirdInspector::ThunderbirdInspector(IRegistry* registry)
{
	m_registry = registry;
}

void ThunderbirdInspector::Execute()
{
	_getVersionAndLocaleFromRegistry();
	_readVersion();
	_readLanguage();
}

void ThunderbirdInspector::_readVersion()
{
	g_log.Log(L"ThunderbirdInspector::_readVersion version %s", (wchar_t*) m_version.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"version", m_version.c_str()));
}

void ThunderbirdInspector::_readLanguage()
{	
	g_log.Log(L"ThunderbirdInspector::_readLanguage language %s", (wchar_t*) m_locale.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"language", m_locale.c_str()));
}

#define THUNDERBIRD_REGKEY L"SOFTWARE\\Mozilla\\Mozilla Thunderbird"

void ThunderbirdInspector::_getVersionAndLocaleFromRegistry()
{
	m_version.clear();
	m_locale.clear();

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, THUNDERBIRD_REGKEY, false) == false)
	{
		g_log.Log(L"ThunderbirdInspector::_getVersionAndLocaleFromRegistry. Cannot open registry key");
		return;
	}
	
	wchar_t szVersion[1024];
	
	if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
	{
		int start, end;
		wstring version= szVersion;

		start = version.find(L" ");
		if (start != wstring::npos)
		{
			m_version = version.substr(0, start);			
			start = version.find(L"(", start);
			if (start != wstring::npos)
			{
				start++;
				end = version.find(L")", start);
				if (end != wstring::npos)
				{
					m_locale = version.substr(start, end-start);
				}
			}
		}
	}

	m_registry->Close();
}