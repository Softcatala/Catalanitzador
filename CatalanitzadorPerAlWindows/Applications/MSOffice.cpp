/* 
 * Copyright (C) 2014-2018 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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


#include <map>
using namespace std;

#define TIMER_ID 2016
 
static map <int, MSOffice*> s_timersIDsToObjs;

#define CATALAN_LCID L"1027" // 0x403
#define VALENCIAN_LCID L"2051" // 0x803
#define CATALAN_PRIMARY_LCID 0x03
#define OFFICE2016_LAP_LANG L"ca-ES"
#define OFFICE2016_LAPKEY L"SOFTWARE\\Microsoft\\Office\\16.0\\Common\\LanguageResources"

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
	m_nTimerID = 0;
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
	_stopTimer();
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

bool MSOffice::_isOffice2016LangAccessoryPackInstalled()
{
	if (m_MSVersion != MSOffice2016_64 && m_MSVersion != MSOffice2016)
	{
		return false;
	}
	
	wstring location;
	_getSHGetFolderPath(location);
	location += L"\\Microsoft Office\\root\\Office16\\1027\\WWINTL.DLL";
	
	bool bFound = GetFileAttributes(location.c_str()) != INVALID_FILE_ATTRIBUTES;
	g_log.Log(L"MSOffice::_isOffice2016LangAccessoryPackInstalled. Found: %u", (wchar_t *) bFound);
	return bFound;
}

void MSOffice::_getSHGetFolderPath(wstring& folder)
{
	wchar_t szProgFolder[MAX_PATH];
	if (m_MSVersion == MSOffice2016_64)
	{
		// Due to WOW all calls always report the "Program Files (x86)" path. Need to look to this environment variable
		ExpandEnvironmentStrings(L"%ProgramW6432%", szProgFolder, ARRAYSIZE(szProgFolder));
	}
	else
	{
		SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES|CSIDL_FLAG_CREATE,  NULL, 0, szProgFolder);
	}
	folder = szProgFolder;
}


bool MSOffice::IsLangPackInstalled()
{	
	bool isInstalled = false;
	RegKeyVersion regkeys = _getRegKeys();
	bool b64bits = (m_MSVersion == MSOffice2010_64 || m_MSVersion == MSOffice2013_64 || m_MSVersion == MSOffice2016_64);

	if (_isOffice2016LangAccessoryPackInstalled())
	{		
		g_log.Log(L"MSOffice::IsLangPackInstalled(Accessory) (%s) 64 bits %u, installed 1", (wchar_t *) GetVersion(),
			(wchar_t*) b64bits);
		return true;
	}

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

void MSOffice::_readDefaultLanguageForOffice2016LangAccessoryPack(bool& isCatalanSetAsDefaultLanguage, bool& followSystemUIOff)
{
	isCatalanSetAsDefaultLanguage = false;
	followSystemUIOff = false;

	if (m_registry->OpenKey(HKEY_CURRENT_USER, OFFICE2016_LAPKEY, false))
	{
		wstring value;
		wchar_t szValue[2048] = L"";
		if (m_registry->GetString(L"UILanguageTag", szValue, sizeof(szValue)))
		{
			value = szValue;
			std::transform(value.begin(), value.end(), value.begin(), ::tolower);
			isCatalanSetAsDefaultLanguage = (value.compare(L"ca-es") == 0 || value.compare(L"ca-es-valencia") == 0);
			g_log.Log(L"MSOffice::_readDefaultLanguageForOffice2016LangAccessoryPack. UILanguageTag: %s, default %u", (wchar_t* )szValue, (wchar_t* )isCatalanSetAsDefaultLanguage);
		}

		// If FollowSystemUI is true Office uses Windows's language pack language to decide which language to use
		DWORD follow = -1;
		if (m_registry->GetDWORD(L"FollowSystemUILanguage", &follow))
		{
			if (follow == 0)
			{
				followSystemUIOff = true;
			}
		}
		m_registry->Close();
	}
	g_log.Log(L"MSOffice::_readDefaultLanguageForOffice2016LangAccessoryPack. isCatalanSetAsDefaultLanguage: %x, FollowSystemUIOff %u",
		(wchar_t* )isCatalanSetAsDefaultLanguage, (wchar_t* )followSystemUIOff);
}

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
	g_log.Log(L"MSOffice::_readDefaultLanguage. isCatalanSetAsDefaultLanguage: %x, FollowSystemUIOff %u",
		(wchar_t* )isCatalanSetAsDefaultLanguage, (wchar_t* )followSystemUIOff);
}


bool MSOffice::IsDefaultLanguage()
{
	// TODO: No language detection for Office 2016
	if (m_MSVersion == MSOffice2016_64 || m_MSVersion == MSOffice2016)
	{
		g_log.Log(L"MSOffice::SetDefaultLanguage. No changes in Office 2016");
		return true;
	}

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
	// TODO: No language setting for Office 2016
	if (m_MSVersion == MSOffice2016_64 || m_MSVersion == MSOffice2016)
	{
		g_log.Log(L"MSOffice::SetDefaultLanguage. No changes in Office 2016");
		return;
	}

	_setDefaultLanguage();	
}

// We install the LIP instead of the LAP then if we call this function is the case
// that the LAP was installed but the language was not selected
void MSOffice::_setDefaultLanguageForOffice2016LangAccessoryPack()
{
	BOOL bSetKey = FALSE;
	
	if (m_registry->OpenKey(HKEY_CURRENT_USER, OFFICE2016_LAPKEY, true))
	{
		bSetKey = m_registry->SetString(L"UILanguageTag", L"ca-es");

		// This key setting tells Office do not use the same language that the Windows UI to determine the Office Language
		// and use the specified language instead
		m_registry->SetDWORD(L"FollowSystemUILanguage", 0);
		m_registry->Close();
	}
	g_log.Log(L"MSOffice::_setDefaultLanguageForOffice2016LangAccessoryPack (%s), set UILanguage %u", (wchar_t*) GetVersion(), (wchar_t *) bSetKey);
}

void MSOffice::_setDefaultLanguage()
{
	BOOL bSetKey = FALSE;
	wchar_t szKeyName [1024];
	int lcid = 0;

	swprintf_s(szKeyName, L"Software\\Microsoft\\Office\\%s\\Common\\LanguageResources", _getRegKeys().VersionNumber);
	if (m_registry->OpenKey(HKEY_CURRENT_USER, szKeyName, true))
	{
		// Always set the language to match the installed langpack
		// GetUseDialectalVariant is only used as guidance when installing a language pack, when setting the language
		// of already existing installation we use the already existing language pack (ca or va) independely of what
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
		case MSOffice2016:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2016_ca_32";			
		case MSOffice2016_64:
			m_packageCodeToSet = CATALAN_LCID;
			return L"2016_ca_64";			
		default:
			return L"";
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

void MSOffice::_startTimer()
{
	m_nTimerID = SetTimer(NULL, TIMER_ID, 500, _timerProc);
	s_timersIDsToObjs[m_nTimerID] = this;
	g_log.Log(L"MSOffice::startTimer");
}

void MSOffice::_stopTimer()
{
	if (m_nTimerID == 0)
		return;

	KillTimer(NULL, m_nTimerID);
	s_timersIDsToObjs.erase(m_nTimerID);
	m_nTimerID = 0;
	g_log.Log(L"MSOffice::stopTimer");
}

VOID CALLBACK MSOffice::_timerProc(HWND hWndTimer, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HWND hWnd;
	map<int, MSOffice*>::iterator it;
	MSOffice* obj;

	it = s_timersIDsToObjs.find(idEvent);

	if (it == s_timersIDsToObjs.end())
		return;

	obj = it->second;

	// Splash screen
	hWnd = FindWindowEx(NULL, NULL, L"Click2RunSplashScreen", NULL);

	while (hWnd)
	{
		wchar_t szText[2048];

		GetWindowText(hWnd, szText, sizeof(szText));
		// Once we send the first HIDE we could stop here, by doing  obj->Stop(), however it is necessary continuing sending HIDE in Adobe case
		ShowWindow(hWnd, SW_HIDE);

		hWnd = FindWindowEx(NULL, hWnd, L"Click2RunSplashScreen", NULL);
	}	

	// installer screen
	hWnd = FindWindowEx(NULL, NULL, L"Click2RunSetupUIClass", NULL);

	while (hWnd)
	{
		wchar_t szText[2048];

		GetWindowText(hWnd, szText, sizeof(szText));
		// Once we send the first HIDE we could stop here, by doing  obj->Stop(), however it is necessary continuing sending HIDE in Adobe case
		ShowWindow(hWnd, SW_HIDE);

		hWnd = FindWindowEx(NULL, hWnd, L"Click2RunSetupUIClass", NULL);
	}
}



void MSOffice::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	switch (m_MSVersion)
	{
		case MSOffice2016_64:
		case MSOffice2016:
			wcscpy_s(szApp, m_szFullFilename);
			_startTimer();
			break;
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
	SetStatus(InProgress);
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
	bool isLangPackAvailable;
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(_getID(),
		wstring(_getDownloadID()));

	isLangPackAvailable = downloadVersion.IsUsable();
	g_log.Log(L"MSOffice::IsLangPackAvailable (%s) available %u", (wchar_t *) GetVersion(), (wchar_t *)isLangPackAvailable);
	return isLangPackAvailable;
}

bool MSOffice::IsNeed()
{
	assert(false);
	return false;
}

void MSOffice::Complete()
{
	if (IsLangPackInstalled())
	{
		SetDefaultLanguage();
		SetStatus(Successful);
	}

	if (IsLangPackAvailable() && IsLangPackInstalled() == false)
	{
		SetStatus(FinishedWithError);
		DumpLogOnError();
	}

	g_log.Log(L"MSOffice::Complete. (%s) is '%s'", (wchar_t *)GetVersion(),	GetStatus() == Successful ? L"Successful" : L"FinishedWithError");
}

void MSOffice::CheckPrerequirements(Action * action)
{
	if (IsLangPackInstalled())
	{
		if (IsDefaultLanguage())
		{
			SetStatus(AlreadyApplied);
		}
	}
	else
	{
		if (IsLangPackAvailable() == false)
		{
			SetStatus(CannotBeApplied);
			return;
		}
	}

	SetStatus(status);
	g_log.Log(L"MSOffice::CheckPrerequirements. (%s) status '%u'", (wchar_t *)GetVersion(), (wchar_t *) GetStatus());
}
