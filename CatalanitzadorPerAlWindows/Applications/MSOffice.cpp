/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "MSOffice.h"
#include "ConfigureLocaleAction.h"
#include "Runner.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

#include <algorithm>

#define CATALAN_LCID L"1027" // 0x403
#define VALENCIAN_LCID L"2051" // 0x803
#define CATALAN_PRIMARY_LCID 0x03


MSOffice::RegKeyVersion MSOffice::RegKeys2003 =
{
	L"11.0", 
	L"SOFTWARE\\Microsoft\\Office\\11.0\\Common\\LanguageResources\\ParentFallback",
	true
};

MSOffice::RegKeyVersion MSOffice::RegKeys2007 = 
{
	L"12.0",
	L"SOFTWARE\\Microsoft\\Office\\12.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

MSOffice::RegKeyVersion MSOffice::RegKeys2010 =
{
	L"14.0",
	L"SOFTWARE\\Microsoft\\Office\\14.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

MSOffice::RegKeyVersion MSOffice::RegKeys2013 =
{
	L"15.0",
	L"SOFTWARE\\Microsoft\\Office\\15.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

MSOffice::RegKeyVersion MSOffice::RegKeys2016 =
{
	L"16.0",
	L"SOFTWARE\\Microsoft\\Office\\16.0\\Common\\LanguageResources\\InstalledUIs",
	false
};

MSOffice::MSOffice(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, MSOfficeVersion version)
{
	m_registry = registry;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_win32I18N = win32I18N;

	m_MSVersion = version;
	m_dialectalVariant = false;

	m_szFullFilename[0] = NULL;
	m_szTempPath2003[0] = NULL;
	m_szFilename[0] = NULL;	
	GetTempPath(MAX_PATH, m_szTempPath);
}

MSOffice::~MSOffice()
{
	if (m_szFullFilename[0] != NULL  && GetFileAttributes(m_szFullFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFullFilename);
	}

	if (m_msiexecLog.empty() == false && GetFileAttributes(m_msiexecLog.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_msiexecLog.c_str());
	}

	_removeOffice2003TempFiles();
}

const wchar_t* MSOffice::GetVersion()
{
	switch (m_MSVersion)
	{
		case MSOffice2003:
			return L"2003";
		case MSOffice2007:
			return L"2007";
		case MSOffice2010:
			return L"2010";
		case MSOffice2010_64:
			return L"2010_64bits";
		case MSOffice2013:
			return L"2013";
		case MSOffice2013_64:
			return L"2013_64bits";
		case MSOffice2016:
			return L"2016";
		case MSOffice2016_64:
			return L"2016_64bits";
		default:
			return L"";
	}
}

// This deletes the contents of the extracted CAB file for MS Office 2003
void MSOffice::_removeOffice2003TempFiles()
{
	if (m_MSVersion != MSOffice2003 || m_szTempPath2003[0] == NULL)
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
			g_log.Log(L"MSOffice::_removeOffice2003TempFiles. Cannot delete '%s'", (wchar_t *) szFile);
		}
	}

	RemoveDirectory(m_szTempPath2003);
}

bool MSOffice::IsLangPackInstalled()
{	
	bool isInstalled = false;
	RegKeyVersion regkeys = _getRegKeys();
	bool b64bits = (m_MSVersion == MSOffice2010_64 || m_MSVersion == MSOffice2013_64 || m_MSVersion == MSOffice2016_64);

	if (b64bits ? m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, regkeys.InstalledLangMapKey, false) :
		m_registry->OpenKey(HKEY_LOCAL_MACHINE, regkeys.InstalledLangMapKey, false))
	{
		m_installedLangPackCode.empty();

		if (regkeys.InstalledLangMapKeyIsDWord)
		{
			DWORD dwValue;
			if (m_registry->GetDWORD(CATALAN_LCID, &dwValue))
			{
				m_installedLangPackCode = CATALAN_LCID;
			}
			else
			{
				if (m_registry->GetDWORD(VALENCIAN_LCID, &dwValue))
				{
					m_installedLangPackCode = VALENCIAN_LCID;
				}
			}
		}
		else
		{
			wchar_t szValue[1024];
			if (m_registry->GetString(CATALAN_LCID, szValue, sizeof (szValue)))
			{
				if (wcslen (szValue) > 0)
				{
					m_installedLangPackCode = CATALAN_LCID;
				}
			}

			if (m_registry->GetString(VALENCIAN_LCID, szValue, sizeof (szValue)))
			{
				if (wcslen (szValue) > 0)
				{
					m_installedLangPackCode = VALENCIAN_LCID;
				}
			}
		}

		if (m_installedLangPackCode.size() > 0)
			isInstalled = true;

		m_registry->Close();
	}

	g_log.Log(L"MSOffice::IsLangPackInstalled (%s) 64 bits %u, installed %u", (wchar_t *) GetVersion(),
		(wchar_t*) b64bits, (wchar_t *)isInstalled);

	return isInstalled;
}

#define UNDEFINED_LCID -1

void MSOffice::_readDefaultLanguage(bool& isCatalanSetAsDefaultLanguage, bool& followSystemUIOff)
{
	wchar_t szKeyName [1024];

	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys().VersionNumber);
	if (m_registry->OpenKey(HKEY_CURRENT_USER, szKeyName, false))
	{
		DWORD lcid = UNDEFINED_LCID;
		if (m_registry->GetDWORD(L"UILanguage", &lcid))
		{			
			if (lcid == _wtoi(VALENCIAN_LCID) || lcid == _wtoi(CATALAN_LCID))
				isCatalanSetAsDefaultLanguage = true;
		}

		// If FollowSystemUI is true Office uses Windows's language pack language to decide which language to use		
		if (m_MSVersion == MSOffice2003)
		{
			followSystemUIOff = (lcid == 0 ? false : true);
		}
		else				
		{
			wstring value;
			wchar_t szValue[2048];
			if (m_registry->GetString(L"FollowSystemUI", szValue, sizeof(szValue)))
			{
				value = szValue;
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);
				if (value.compare(L"off") == 0)
					followSystemUIOff = true;
			}
		}
		m_registry->Close();
	}
	g_log.Log(L"MSOffice::_readDefaultLanguage. isCatalanSetAsDefaultLanguage: %x, FollowSystemUI %u",
		(wchar_t* )isCatalanSetAsDefaultLanguage, (wchar_t* )followSystemUIOff);
}


bool MSOffice::IsDefaultLanguage()
{
	ConfigureLocaleAction configureLocaleAction((IOSVersion*) NULL, m_registry, (IRunner*)NULL);
	bool isDefaultLanguage = false;
	bool isCatalanSetAsDefaultLanguage = false;
	bool followSystemUIOff = false;
	
	_readDefaultLanguage(isCatalanSetAsDefaultLanguage, followSystemUIOff);
	
	if (followSystemUIOff)
	{
		if (isCatalanSetAsDefaultLanguage)
			isDefaultLanguage = true;
	}
	else
	{
		 if (m_win32I18N->GetUserDefaultUILanguage() == CATALAN_PRIMARY_LCID)
			isDefaultLanguage = true;
	}

	g_log.Log(L"MSOffice::IsDefaultLanguage (%s) returns '%u'", (wchar_t*) GetVersion(), (wchar_t*) isDefaultLanguage);
	return isDefaultLanguage;
}

void MSOffice::SetDefaultLanguage()
{
	BOOL bSetKey = FALSE;
	wchar_t szKeyName [1024];
	int lcid = 0;

	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys().VersionNumber);
	if (m_registry->OpenKey(HKEY_CURRENT_USER, szKeyName, true))
	{		

		// Always set the language to match the installed langpack
		// GetUseDialectalVariant is only used as guidance when installing a language pack, when setting the language
		// of already existing installation we use the already exisitng language pack (ca or va) indepedenly of what
		// the user has selected
		if (m_packageCodeToSet.size() > 0)
		{
			lcid = _wtoi(m_packageCodeToSet.c_str());
		}
		else
		{
			lcid = _wtoi(m_installedLangPackCode.c_str());
		}

		bSetKey = m_registry->SetDWORD(L"UILanguage", lcid);

		// This key setting tells Office do not use the same language that the Windows UI to determine the Office Language
		// and use the specified language instead
		if (m_MSVersion != MSOffice2003)
		{
			m_registry->SetString(L"FollowSystemUI", L"Off");
		}		
		m_registry->Close();
	}
	g_log.Log(L"MSOffice::SetDefaultLanguage (%s), set UILanguage %u, lcid %u", (wchar_t*) GetVersion(), (wchar_t *) bSetKey, (wchar_t *) lcid);
}

MSOffice::RegKeyVersion MSOffice::_getRegKeys()
{
	switch (m_MSVersion)
	{
		case MSOffice2003:
			return RegKeys2003;
		case MSOffice2007:
			return RegKeys2007;
		case MSOffice2010:
		case MSOffice2010_64:
			return RegKeys2010;
		case MSOffice2013:
		case MSOffice2013_64:
			return RegKeys2013;
		case MSOffice2016:		
		case MSOffice2016_64:
		default:
			return RegKeys2016;
	}
}

wchar_t*  MSOffice::_getDownloadID()
{
	switch (m_MSVersion)
	{
		case MSOffice2003:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2003";
		case MSOffice2007:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2007";
		case MSOffice2010:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2010_32";
		case MSOffice2010_64:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2010_64";
		case MSOffice2013:
			if (GetUseDialectalVariant())
			{
				m_packageCodeToSet = VALENCIAN_LCID;
				return L"2013_va_32";
			}
			else
			{
				m_packageCodeToSet = CATALAN_LCID;
				return L"2013_ca_32";
			}			
		case MSOffice2013_64:
			if (GetUseDialectalVariant())
			{
				m_packageCodeToSet = VALENCIAN_LCID;
				return L"2013_va_64";
			}
			else
			{
				m_packageCodeToSet = CATALAN_LCID;
				return L"2013_ca_64";
			}
		default:
			return NULL;
	}
}

bool MSOffice::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");

	swprintf_s (szParams, L" %s %s -f:*", file, path);
	g_log.Log(L"MSOffice::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute(szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

void MSOffice::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	switch (m_MSVersion)
	{
		case MSOffice2013_64:
		case MSOffice2013:
		case MSOffice2010_64:
		case MSOffice2010:		
		{
			wchar_t logFile[MAX_PATH];

			wcscpy_s(szApp, m_szFullFilename);
			wcscpy_s(szParams, L" /passive /norestart /quiet");

			// We need temporary unique files for the logs since we can execute several MS instances
			GetTempFileName(m_szTempPath, L"mslog", 0, logFile);
			wcscat_s(szParams, L" /log:");
			wcscat_s(szParams, logFile);
			m_msiexecLog = logFile;
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
				g_log.Log(L"MSOffice::Execute. Cannot create temp directory '%s'", m_szTempPath2003);
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

	g_log.Log(L"MSOffice::Execute (%s) '%s' with params '%s'", (wchar_t*) GetVersion(), szApp, szParams);
	m_runner->Execute(szApp, szParams);
}

void MSOffice::AddDownloads(MultipleDownloads& multipleDownloads)
{	
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(_getID(),
		wstring(_getDownloadID()));

	wcscpy_s(m_szFilename, downloadVersion.GetFilename().c_str());
	wcscpy_s(m_szFullFilename, m_szTempPath);
	wcscat_s(m_szFullFilename, m_szFilename);
	multipleDownloads.AddDownload(downloadVersion, m_szFullFilename);
}


#define LINES_TODUMP 7
#define KEYWORD_TOSEARCH L"Error"

void MSOffice::DumpLogOnError()
{
	if (m_msiexecLog.empty() == true)
		return;

	LogExtractor logExtractor(m_msiexecLog, LINES_TODUMP);
	logExtractor.SetExtractLastOccurrence(true);
	logExtractor.SetFileIsUnicode(false);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
	logExtractor.DumpLines();	
}

bool MSOffice::IsLangPackAvailable()
{
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(_getID(),
		wstring(_getDownloadID()));

	return downloadVersion.IsUsable();
}




