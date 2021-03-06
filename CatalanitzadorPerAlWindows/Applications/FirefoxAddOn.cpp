/* 
 * Copyright (C) 2013-2017 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
#include "FirefoxAddOn.h"
#include "FirefoxPreferencesFile.h"

#define ADDON_ID L"\"id\""
const int ADDON_ID_LEN = wcslen(ADDON_ID);

#define STRING_DELIMITER L"\""
const int STRING_DELIMITER_LEN = wcslen(STRING_DELIMITER);

// Mozilla products support extensions for different Mozilla Apps (e.g. Thunderbird)
// We will be always using Firefox 
const wchar_t* FIREFOX_EXTENSION_ID_GUID = L"{ec8030f7-c20a-464f-9b0e-13a3a9e97384}";

FirefoxAddOn::FirefoxAddOn(wstring userDataDirectory, wstring profileRootDir, wstring installPath)
{
	m_userDataDirectory = userDataDirectory;
	m_profileRootDir = profileRootDir;
	m_installPath = installPath;
}

void FirefoxAddOn::_writeConfigJSFile()
{
	char szBuff[] = "pref('general.config.filename', 'mozilla.cfg');\r\npref('general.config.obscure_value', 0);";

	wstring filename = m_installPath + L"\\defaults\\pref\\local.js";
	ofstream of(filename.c_str());
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();

	g_log.Log(L"FirefoxAddOn::_writeConfigJSFile. Write to '%s'",  (wchar_t *)filename.c_str());

}

void FirefoxAddOn::_writeMozillaCfg()
{
	char szBuff[] = "// required empty comment\r\n\r\ndefaultPref(\"extensions.autoDisableScopes\", 0);\r\ndefaultPref(\"extensions.enabledScopes\", 15);";

	wstring filename = m_installPath + L"\\mozilla.cfg";
	ofstream of(filename.c_str());
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();

	g_log.Log(L"FirefoxAddOn::_writeMozillaCfg. Write to '%s'",  (wchar_t *)filename.c_str());
}

// https://developer.mozilla.org/en-US/Add-ons/WebExtensions/Alternative_distribution_options/Add-ons_in_the_enterprise
wstring FirefoxAddOn::InstallAddOn(wstring applicationID, wstring file)
{
	wstring extensionsDirectory, targetfile;
	wchar_t szPath[MAX_PATH];
	wstring path;
	FirefoxPreferencesFile preferences(m_profileRootDir);

	preferences.GetPreferencesDirectory(path);
	// Create directory if it is necessary
	swprintf_s(szPath, L"%s\\extensions", path.c_str());

	if (GetFileAttributes(szPath) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectory(szPath, NULL);
	}

	swprintf_s(szPath, L"%s\\extensions\\%s.xpi", path.c_str(), applicationID.c_str());
	if (CopyFile(file.c_str(), szPath, false) == FALSE)
	{
		g_log.Log(L"FirefoxAddOn::InstallAddOn. Unable to copy extension to '%s'", szPath);
		return wstring();
	}
	_writeConfigJSFile();
	_writeMozillaCfg();
	return wstring(szPath);
}

// 
// The files addons.sqlite (stores the information that appears in the Addons tab > Extensions) and extensions.sqlite (stores data about installed extensions)
// are the places where the extensions information is stored. Instead of reading these files and entering into SqlLite dependencies, we look for the 
// extensions files on disk
bool FirefoxAddOn::IsAddOnInstalled(wstring applicationID)
{
	wstring path;
	wifstream reader;
	FirefoxPreferencesFile preferences(m_profileRootDir);
	wchar_t szPath[MAX_PATH];

	// Is extension installed in profile/extension directory? (location used when user installs the extension)
	if (preferences.GetPreferencesDirectory(path) == false)
	{
		g_log.Log(L"FirefoxAddOn::IsAddOnInstalled. No preferences directory");
		return false;
	}

	swprintf_s(szPath, L"%s\\extensions\\%s.xpi", path.c_str(), applicationID.c_str());
	if (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}

	// Is extension installed in m_userDataDirectory/extension directory? (location used for automated installations)
	swprintf_s(szPath, L"%sExtensions\\%s\\%s.xpi", m_userDataDirectory.c_str(), FIREFOX_EXTENSION_ID_GUID, applicationID.c_str());
	if (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}	
	return false;
}

bool FirefoxAddOn::CanInstallAddOns()
{
	FirefoxPreferencesFile preferences(m_profileRootDir);
	bool canInstall;
	wstring path;
	
	canInstall = preferences.GetPreferencesDirectory(path);
	g_log.Log(L"FirefoxAddOn::CanInstallAddOns. Result %u", (wchar_t*) canInstall);
	return canInstall;
}