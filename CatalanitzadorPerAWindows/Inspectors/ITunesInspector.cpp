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
#include "ITunesInspector.h"

ITunesInspector::ITunesInspector(IRegistry* registry)
{
	m_registry = registry;
}

#define PROGRAM_REGKEY L"Software\\Apple Computer, Inc.\\iTunes"
#define UNINSTALL_REGKEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

void ITunesInspector::_readLangInstalled()
{
	wchar_t szLanguage[1024] = L"";
	bool found = false;
	
	if ( m_registry->OpenKey(HKEY_CURRENT_USER, PROGRAM_REGKEY, false))
	{
		m_registry->GetString(L"LangID", szLanguage, sizeof(szLanguage));
		if (wcslen(szLanguage) > 0){
			found = true;
		}
		m_registry->Close();
	}
	
	if ( (!found) && ( m_registry->OpenKey(HKEY_LOCAL_MACHINE, PROGRAM_REGKEY, false)) )
	{
		m_registry->GetString(L"InstalledLangID", szLanguage, sizeof(szLanguage));
		m_registry->Close();	
	}
	
	g_log.Log(L"ITunesInspector::_readLangInstalled '%s'", szLanguage);
	m_KeyValues.push_back(InspectorKeyValue(L"lang", szLanguage));
	
}

void ITunesInspector::Execute()
{
	_readVersion();
	_readLangInstalled();	
}

void ITunesInspector::_enumInstalledSoftware(vector <wstring>& apps)
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, UNINSTALL_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;
			
			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				apps.push_back(key);
			}
		}
	}
	m_registry->Close();
}


void ITunesInspector::_readVersion()
{	
	vector <wstring> apps;
	wchar_t szName[1024] = L"";
	wchar_t szVersion[1024] = L"";
	bool bFound = false;

	_enumInstalledSoftware(apps);

	int i=0;
	while (i < apps.size() && (!bFound) )
	{
		wstring szFirstKey = UNINSTALL_REGKEY;
		wstring szSecondKey = L"\\"+ apps[i];
		wstring szKey = szFirstKey + szSecondKey;

		i++;

		if (m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, (wchar_t*)szKey.c_str(), false))
		{
			m_registry->GetString(L"DisplayName", szName, sizeof(szName));

			if (!wcscmp(szName, L"iTunes"))
			{
				m_registry->GetString(L"DisplayVersion", szVersion, sizeof(szVersion));
				bFound = true;
			}
			m_registry->Close();
		}
	}
	
	g_log.Log(L"ITunesInspector::_readVersionInstalled '%s'", szVersion);
	m_KeyValues.push_back(InspectorKeyValue(L"version", szVersion));		
}

