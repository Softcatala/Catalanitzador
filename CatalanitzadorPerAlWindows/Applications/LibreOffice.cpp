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
#include "LibreOffice.h"

#define LIBREOFFICE_REGKEY L"SOFTWARE\\LibreOffice\\LibreOffice"

LibreOffice::LibreOffice(IRegistry* registry)
{
	m_installationPathRead = false;
	m_registry = registry;
}

bool LibreOffice::IsInstalled()
{
	if (m_isInstalled.IsUndefined())
	{
		if (_readLibreOfficeVersionInstalled())
		{
			m_isInstalled = m_version.size() > 0;
		}
		else
		{
			m_isInstalled = false;
		}
	}

	return m_isInstalled == true;
}

wstring LibreOffice::GetVersion()
{
	if (m_isInstalled.IsUndefined())
	{
		_readLibreOfficeVersionInstalled();		
	}

	return m_version;
}

wstring LibreOffice::GetInstallationPath()
{
	
	if (m_installationPathRead == false)
	{
		_readLibreOfficeVersionInstalled();
	}
	return m_installationPath;
}




void LibreOffice::_readLibreOfficeInstallPath(wstring& path)
{
	if (m_installationPathRead)
		return;

	m_installationPathRead = true;
	wstring key;
	bool bRslt = false;

	key = LIBREOFFICE_REGKEY;
	key += L"\\";
	key += m_version;

	path.erase();
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false))
	{
		wchar_t szFileName[MAX_PATH];

		if (m_registry->GetString(L"path", szFileName, sizeof(szFileName)))
		{
			// This is the path to the executable, remove filename at the end
			int i;
			for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);

			szFileName[i + 1] = NULL;
			path = szFileName;
		}
		m_registry->Close();
	}
	g_log.Log(L"LibreOffice::_isLangPackInstalled '%u'", (wchar_t *) bRslt);
	//return bRslt;
}

bool LibreOffice::_readLibreOfficeVersionInstalled()
{
	bool bKeys = true;
	DWORD dwIndex = 0;
	
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, LIBREOFFICE_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				m_version = key;
				break;
			}
		}
		m_registry->Close();
	}
	g_log.Log(L"LibreOffice::_readLibreOfficeVersionInstalled '%s'", (wchar_t *) m_version.c_str());	
	return m_version.empty() == false;
}
