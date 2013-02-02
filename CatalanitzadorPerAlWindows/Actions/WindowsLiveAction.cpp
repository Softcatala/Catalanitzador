/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "WindowsLiveAction.h"
#include "Winver.h"
#include "Url.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

#define MS_LIVE_ESSENTIALS_2009 14
#define CATALAN_WINLANGCODE 3

WindowsLiveAction::WindowsLiveAction(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;
	m_runner = runner;
	m_fileVersionInfo = fileVersionInfo;
	m_szFilename[0]=NULL;
}

WindowsLiveAction::~WindowsLiveAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

wchar_t* WindowsLiveAction::GetName()
{	
	return _getStringFromResourceIDName(IDS_WINDOWSLIVE_NAME, szName);	
}

wchar_t* WindowsLiveAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLIVE_DESCRIPTION, szDescription);
}

bool WindowsLiveAction::IsNeed()
{
	bool bNeed;

	switch(GetStatus())
	{		
		case NotInstalled:
		case AlreadyApplied:
		case CannotBeApplied:
			bNeed = false;
			break;
		default:
			bNeed = true;
			break;
	}

	g_log.Log(L"WindowsLiveAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

bool WindowsLiveAction::Download(ProgressStatus progress, void *data)
{
	ConfigurationFileActionDownload downloadVersion;
	wchar_t version[32];

	swprintf_s(version, L"%u", _getMajorVersion());
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(version));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());
	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}

const wchar_t* WindowsLiveAction::GetVersion()
{
	if (m_version.length() == 0)
	{
		_readVersionInstalled();
	}
	return m_version.c_str();
}

void WindowsLiveAction::_readVersionInstalled()
{
	if (m_version.size() > 0)
		return;

	wstring location;

	_getInstallerLocation(location);	
	m_fileVersionInfo->SetFilename(location);
	m_version = m_fileVersionInfo->GetVersion();
}

void WindowsLiveAction::_getInstallerLocation(wstring& location)
{
	wchar_t szPath[MAX_PATH];

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Windows Live\\Installer\\wlarp.exe";
}

void WindowsLiveAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";
	wstring location;

	_getInstallerLocation(location);

	// Live Essential 2009/2010/2011 parameters: http://www.mydigitallife.info/windows-live-essentials-unattended-silent-setup-installation-switches/
	wcscpy_s(szApp, m_szFilename);
	// By selecting only Silverlight the installer will update only the installer apps
	// instead of installing also new ones. This saves us to dectect which exact component
	// is installed and uninstall it
	wcscat_s(szApp, L" /AppSelect:Silverlight /quiet");	

	SetStatus(InProgress);
	g_log.Log(L"WindowsLiveAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(NULL, szApp);
}

ActionStatus WindowsLiveAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangSelected())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
			_dumpWLSetupErrors();
		}
		
		g_log.Log(L"WindowsLiveAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

int WindowsLiveAction::_getMajorVersion()
{
	wstring location;

	_getInstallerLocation(location);
	m_fileVersionInfo->SetFilename(location);
	return m_fileVersionInfo->GetMajorVersion();
}

#define LANG_REGKEY L"Software\\Microsoft\\Windows Live\\Common\\"
#define LANGUAGE_CODE L"CA"

bool WindowsLiveAction::_isLangSelected2011()
{
	bool bSelected = false;
	wchar_t szLanguage[MAX_PATH] = L"";

	if (m_registry->OpenKey(HKEY_CURRENT_USER, LANG_REGKEY, false))
	{
		if (m_registry->GetString(L"UserLanguage", szLanguage, sizeof(szLanguage)))
		{
			bSelected = _wcsnicmp(szLanguage, LANGUAGE_CODE, wcslen(LANGUAGE_CODE)) == 0;
		}
		m_registry->Close();
	}
	g_log.Log(L"WindowsLiveAction::_isLangSelected2011. Language %s", szLanguage);
	return bSelected;
}

bool WindowsLiveAction::_isLangSelected2009()
{
	wchar_t szPath[MAX_PATH];
	wstring location;
	DWORD langCode;

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Windows Live\\Installer\\wlsres.dll";
	
	m_fileVersionInfo->SetFilename(location);
	langCode = m_fileVersionInfo->GetLanguageCode();

	g_log.Log(L"WindowsLiveAction::_isLangSelected2009. Language '%u'", (wchar_t *)langCode);
	return langCode == CATALAN_WINLANGCODE;
}

bool WindowsLiveAction::_isLangSelected()
{
	if (_getMajorVersion() == MS_LIVE_ESSENTIALS_2009)
	{
		return _isLangSelected2009();
	}

	return _isLangSelected2011();
}

#define REBOOT_REQUIRED_KEY L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired"

bool WindowsLiveAction::_isRebootRequired()
{
	if (_getMajorVersion() == MS_LIVE_ESSENTIALS_2009)
	{
		return false;
	}

	bool rebootRequiered = false;
	
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, REBOOT_REQUIRED_KEY, false))
	{
		wstring value;

		rebootRequiered = m_registry->RegEnumValue(0, value);
		m_registry->Close();
	}
	return rebootRequiered;
}

bool WindowsLiveAction::_isDownloadAvailable()
{
	wchar_t version[32];
	ConfigurationFileActionDownload downloadVersion;

	swprintf_s(version, L"%u", _getMajorVersion());
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(version));

	return downloadVersion.IsEmpty() == false;
}

void WindowsLiveAction::CheckPrerequirements(Action * action)
{	
	_readVersionInstalled();
	
	if (m_version.size() > 0)
	{	
		if (_isLangSelected() == true)
		{
			SetStatus(AlreadyApplied);
			return;
		}

		if (_isDownloadAvailable() == false)
		{
			_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"WindowsLiveAction::CheckPrerequirements. Version not supported");
			SetStatus(CannotBeApplied);
			return;
		}

		if (_isRebootRequired())
		{
			_getStringFromResourceIDName(IDS_REBOOTREQUIRED, szCannotBeApplied);
			g_log.Log(L"WindowsLiveAction::CheckPrerequirements. Reboot required");
			SetStatus(CannotBeApplied);
			return;
		}
	}
	else
	{
		_setStatusNotInstalled();
		return;
	}
}

#define KEYWORD_TOSEARCH L"Error"
#define LINES_TODUMP 7
#define LOGS_PATH L"\\Microsoft\\WLSetup\\Logs\\"

wstring WindowsLiveAction::_getMostRecentWLSetupLogFile()
{
	HANDLE hFind;
	WIN32_FIND_DATA findFileData;
	wchar_t szLogPath[MAX_PATH];
	wchar_t szFilename[MAX_PATH];
	wchar_t szRecentFilename[MAX_PATH];
	FILETIME filetime = {0};
	wstring filename;	

	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szLogPath);
	wcscat_s(szLogPath, LOGS_PATH);

	wcscpy_s(szFilename, szLogPath);
	wcscat_s(szFilename, L"*.log");
	
	hFind = FindFirstFile(szFilename, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		g_log.Log(L"WindowsLiveAction::_getMostRecentWLSetupLogFile. Cannot open 's%'", szFilename);
		return wstring();
	}

	// If there are multiple files, select the most recent one
	do
	{
		if (findFileData.ftLastWriteTime.dwHighDateTime > filetime.dwHighDateTime ||
			findFileData.ftLastWriteTime.dwHighDateTime == filetime.dwHighDateTime && findFileData.ftLastWriteTime.dwLowDateTime > filetime.dwLowDateTime)
		{
			wcscpy_s(szRecentFilename, findFileData.cFileName);
			filetime = findFileData.ftLastWriteTime;
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	
	filename = szLogPath;
	filename += szRecentFilename;
	return filename;
}

void WindowsLiveAction::_dumpWLSetupErrors()
{
	wstring filename;

	filename = _getMostRecentWLSetupLogFile();

	if (filename.empty())
	{
		return;
	}

	Sleep(10000); // Since the WLSetup.exe process exists until you can read the file it takes some seconds
	LogExtractor logExtractor(filename, LINES_TODUMP);
	logExtractor.SetFileIsUnicode(false);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
	logExtractor.DumpLines();
}