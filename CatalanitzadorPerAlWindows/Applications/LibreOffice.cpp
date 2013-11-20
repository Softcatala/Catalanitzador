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
#include "TempFile.h"
#include "XmlParser.h"
#include "ApplicationVersion.h"
#include <algorithm>

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
		_readLibreOfficeInstallPath();
	}
	return m_installationPath;
}

void LibreOffice::_readLibreOfficeInstallPath()
{
	if (m_installationPathRead)
		return;

	m_installationPathRead = true;
	wstring key;
	bool bRslt = false;

	key = LIBREOFFICE_REGKEY;
	key += L"\\";
	key += GetVersion();

	m_installationPath.erase();
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false))
	{
		wchar_t szFileName[MAX_PATH];

		if (m_registry->GetString(L"path", szFileName, sizeof(szFileName)))
		{
			// This is the path to the executable, remove filename at the end
			int i;
			for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);

			szFileName[i + 1] = NULL;
			m_installationPath = szFileName;
		}
		m_registry->Close();
	}
	g_log.Log(L"LibreOffice::_readLibreOfficeInstallPath '%s'", (wchar_t *) m_installationPath.c_str());
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

wstring LibreOffice::_getAppDataDir()
{
	wchar_t szPath[MAX_PATH];	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	return wstring(szPath);
}

wstring LibreOffice::_getPreferencesFile()
{
	wchar_t directory[1024];
	wstring location;
	ApplicationVersion applicationVersion(GetVersion());
	
	location = _getAppDataDir();
	// This is based on the assumption that directory changes with every major version happened in versions 3 and 4
	swprintf_s(directory, L"\\LibreOffice\\%u\\user\\", applicationVersion.GetMajorVersion());	
	location += directory;
	return location;
}

wstring LibreOffice::_getExtensionsFile()
{
	wstring location;
	location = _getPreferencesFile();
	location += L"uno_packages\\cache\\registry\\com.sun.star.comp.deployment.component.PackageRegistryBackend\\backenddb.xml";
	return location;
}

bool LibreOffice::_readNodeCallback(XmlNode node, void *data)
{
	if (node.GetName().compare(L"comp:name")==0)
	{
		vector <wstring>* extensions = (vector <wstring>*) data;
		wstring text = node.GetText();
		extensions->push_back(text);
	}	
	return true;
}

void LibreOffice::_parseXmlConfiguration(vector <wstring>& extensions)
{
	wstring preferences;

	preferences = _getPreferencesFile();
	preferences += L"uno_packages\\cache\\registry\\com.sun.star.comp.deployment.component.PackageRegistryBackend\\backenddb.xml";

	XmlParser parser;

	if (parser.Load(preferences) == false)
	{
		g_log.Log(L"LibreOffice::_parseXmlConfiguration. Could not open '%s'", (wchar_t *) preferences.c_str());
		return;
	}

	parser.Parse(_readNodeCallback, &extensions);
}

void LibreOffice::_readListOfExtensions(vector <wstring>& extensions)
{	
	_parseXmlConfiguration(extensions);
	return;
}

bool LibreOffice::IsExtensionInstalled(wstring extension)
{
	vector <wstring> extensions;

	_readListOfExtensions(extensions);

	for (unsigned int i = 0; i < extensions.size(); i++)
	{
		wstring e = extensions.at(i);
		if (e == extension)
			return true;
	}
	return false;
}