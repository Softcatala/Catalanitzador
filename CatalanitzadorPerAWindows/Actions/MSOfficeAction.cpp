/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include <stdio.h>

#include "MSOfficeAction.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Registry.h"
#include "Url.h"


RegKeyVersion RegKeys2003 = 
{
	L"11.0", 
	L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources\\ParentFallback",	
	true
};

RegKeyVersion RegKeys2007 = 
{
	L"12.0",
	L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

RegKeyVersion RegKeys2010 = 
{
	L"14.0",
	L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs",
	false
};


MSOfficeAction::MSOfficeAction()
{
	m_bLangPackInstalled = false;
	m_szFullFilename[0] = NULL;
	m_szTempPath2003[0] = NULL;
	_getVersionInstalled();
	GetTempPath(MAX_PATH, m_szTempPath);

	Url url(m_downloadAction.GetFileName(_getDownloadID()));
	wcscpy_s(m_szFilename, url.GetFileName());	
}

MSOfficeAction::~MSOfficeAction()
{
	if (m_szFullFilename[0] != NULL  && GetFileAttributes (m_szFullFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFullFilename);
	}

	_removeOffice2003TempFiles();
}

wchar_t* MSOfficeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_NAME, szName);
}

wchar_t* MSOfficeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_DESCRIPTION, szDescription);
}

char* MSOfficeAction::GetVersion()
{
	switch (m_MSVersion)
	{
		case MSOffice2003:
			return "2003";
		case MSOffice2007:
			return "2007";
		case MSOffice2010:
			return "2010";
		default:
			return "";
	}
}

// This deletes the contents of the extracted CAB file for MS Office 2003
void MSOfficeAction::_removeOffice2003TempFiles()
{
	if (m_bLangPackInstalled == true || m_MSVersion != MSOffice2003)
		return;
	
	wchar_t szFile[MAX_PATH];
	wchar_t* files[] = {L"DW20.ADM_1027", L"DWINTL20.DLL_0001_1027", L"LIP.MSI",
		L"lip.xml", L"OSE.EXE", L"SETUP.CHM_1027", L"SETUP.EXE", L"SETUP.INI", L"lip1027.cab",
		NULL};

	for(int i = 0; files[i] != NULL; i++)
	{
		wcscpy_s(szFile, m_szTempPath2003);
		wcscat_s(szFile, L"\\");
		wcscat_s(szFile, files[i]);

		if (DeleteFile(szFile) == FALSE)
		{
			g_log.Log(L"MSOfficeAction::_removeOffice2003TempFiles. Cannot delete '%s'", (wchar_t *) szFile);
		}		
	}

	RemoveDirectory(m_szTempPath2003);
}

bool MSOfficeAction::_isVersionInstalled(RegKeyVersion regkeys)
{	
	Registry registry;
	wchar_t szValue[1024];
	wchar_t szKey[1024];
	bool Installed = false;

	swprintf_s(szKey, L"SOFTWARE\\Microsoft\\Office\\%s\\Common\\InstallRoot", regkeys.VersionNumber);
	if (registry.OpenKey(HKEY_LOCAL_MACHINE, szKey, false))
	{
		if (registry.GetString(L"Path", szValue, sizeof (szValue)))
		{
			if (wcslen(szValue) > 0)
				Installed = true;
		}
		registry.Close();
	}
	return Installed;
}

bool MSOfficeAction::_isLangPackForVersionInstalled(RegKeyVersion regkeys)
{
	Registry registry;	
	bool Installed = false;

	if (registry.OpenKey(HKEY_LOCAL_MACHINE, regkeys.InstalledLangMapKey, false))
	{		
		if (regkeys.InstalledLangMapKeyIsDWord)
		{
			DWORD dwValue;
			if (registry.GetDWORD(L"1027", &dwValue))
					Installed = true;
		}		
		else
		{
			wchar_t szValue[1024];
			if (registry.GetString(L"1027", szValue, sizeof (szValue)))
			{
				if (wcslen (szValue) > 0)
					Installed = true;
			}
		}
		registry.Close ();
	}
	return Installed;
}

void MSOfficeAction::_getVersionInstalled()
{
	wchar_t szVersion[256];
	char* pVersion;

	if (_isVersionInstalled(RegKeys2010))
	{
		m_MSVersion = MSOffice2010;
		m_bLangPackInstalled = _isLangPackForVersionInstalled(RegKeys2010);
	} else if (_isVersionInstalled(RegKeys2007))
	{
		m_MSVersion = MSOffice2007;
		m_bLangPackInstalled = _isLangPackForVersionInstalled(RegKeys2007);
	} else if (_isVersionInstalled(RegKeys2003))
	{
		m_MSVersion = MSOffice2003;
		m_bLangPackInstalled = _isLangPackForVersionInstalled(RegKeys2003);
	} else {
		m_bLangPackInstalled = false;
		m_MSVersion = NoMSOffice;
		_getStringFromResourceIDName(IDS_MSOFFICEACTION_NOOFFICE, szCannotBeApplied);
	}

	pVersion = GetVersion();

	if (strlen(pVersion) > 0)
	{
		MultiByteToWideChar(CP_ACP, 0,  pVersion, strlen (pVersion) + 1, szVersion, sizeof (szVersion));
	}
	else
	{
		wcscpy_s(szVersion, L"None");
	}

	g_log.Log(L"MSOfficeAction::_getVersionInstalled '%s' installed langmap '%u'", szVersion, (wchar_t *)m_bLangPackInstalled);
}

DownloadID  MSOfficeAction::_getDownloadID()
{
	switch (m_MSVersion)
	{
		case MSOffice2010:
			return DI_MSOFFICEACTION_2010;
		case MSOffice2007:
			return DI_MSOFFICEACTION_2007;
		case MSOffice2003:
			return DI_MSOFFICEACTION_2003;
		default:
			break;
	}
	return DI_UNKNOWN;
}

bool MSOfficeAction::IsNeed()
{
	bool bNeed;

	bNeed = m_bLangPackInstalled == false && m_MSVersion != NoMSOffice;

	if (bNeed == false)
	{
		if (m_MSVersion == NoMSOffice)
			status = CannotBeApplied;
		else
			status = AlreadyApplied;
	}

	g_log.Log(L"MSOfficeAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool MSOfficeAction::Download(ProgressStatus progress, void *data)
{
	wcscpy_s(m_szFullFilename, m_szTempPath);	
	wcscat_s(m_szFullFilename, m_szFilename);
	return _getFile(_getDownloadID(), m_szFullFilename, progress, data);
}

bool MSOfficeAction::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");

	swprintf_s (szParams, L" %s %s -f:*", m_szFullFilename, path);
	g_log.Log(L"MSOfficeAction::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute (szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

void MSOfficeAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	switch (m_MSVersion)
	{
		case MSOffice2010:
		{
			wcscpy_s(szApp, m_szFullFilename);
			wcscpy_s(szParams, L" /passive /norestart /quiet");
			break;
		}
		case MSOffice2007:
		{
			wcscpy_s(szApp, m_szFullFilename);
			wcscpy_s(szParams, L" /quiet");
			break;
		}
		case MSOffice2003:
		{
			wchar_t szMSI[MAX_PATH];

			// Unique temporary file (needs to create it)
			GetTempFileName(m_szTempPath, L"CAT", 0, m_szTempPath2003);
			DeleteFile(m_szTempPath2003);

			if (CreateDirectory(m_szTempPath2003, NULL) == FALSE)
			{
				g_log.Log(L"MSOfficeAction::Execute. Cannot create temp directory '%s'", m_szTempPath2003);
				break;
			}
		
			_extractCabFile(m_szFullFilename, m_szTempPath2003);

			GetSystemDirectory(szApp, MAX_PATH);
			wcscat_s(szApp, L"\\msiexec.exe ");

			wcscpy_s(szMSI, m_szTempPath2003);
			wcscat_s(szMSI, L"\\");
			wcscat_s(szMSI, L"lip.msi");

			wcscpy_s(szParams, L" /i ");
			wcscat_s(szParams, szMSI);
			wcscat_s(szParams, L" /qn");
			break;
		}
	default:
		break;
	}

	status = InProgress;
	g_log.Log(L"MSOfficeAction::Execute '%s' with params '%s'", szApp, szParams);
	runner.Execute (szApp, szParams);	
}

bool MSOfficeAction::_wasInstalledCorrectly()
{	
	return _isLangPackForVersionInstalled(_getRegKeys());	
}

RegKeyVersion MSOfficeAction::_getRegKeys()
{
	switch (m_MSVersion)
	{
		case MSOffice2003:
			return RegKeys2003;
		case MSOffice2007:
			return RegKeys2007;
		case MSOffice2010:
		default:
			return RegKeys2010;
	}
}

void MSOfficeAction::_setDefaultLanguage()
{
	wchar_t szKeyName [1024];
	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys().VersionNumber);

	Registry registry;
	BOOL bSetKey = FALSE;

	if (registry.OpenKey(HKEY_CURRENT_USER, szKeyName, true))
	{
		bSetKey = registry.SetDWORD(L"UILanguage", 1027);

		// This key setting tells Office do not use the same language that the Windows UI to determine the Office Language
		// and use the specified language instead
		if (m_MSVersion == MSOffice2010 || m_MSVersion == MSOffice2007)
		{
			BOOL bSetFollowKey = registry.SetString(L"FollowSystemUI", L"Off");
			g_log.Log(L"MSOfficeAction::_setDefaultLanguage, set FollowSystemUI %u", (wchar_t *) bSetFollowKey);	
		}		
		registry.Close();
	}
	g_log.Log(L"MSOfficeAction::_setDefaultLanguage, set UILanguage %u", (wchar_t *) bSetKey);	
}

ActionStatus MSOfficeAction::GetStatus()
{
	if (status == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		if (_wasInstalledCorrectly()) {
			status = Successful;
			_setDefaultLanguage();
		}
		else {
			status = FinishedWithError;
		}		
		g_log.Log(L"MSOfficeAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}
