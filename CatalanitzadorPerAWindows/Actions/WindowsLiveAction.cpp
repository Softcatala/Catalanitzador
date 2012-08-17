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
#include <Shlobj.h>

#include "WindowsLiveAction.h"
#include "Winver.h"
#include "Url.h"
#include "ConfigurationInstance.h"

#define MS_LIVE_ESSENTIALS_2009 14
#define MS_LIVE_ESSENTIALS_2011 15
#define CATALAN_WINLANGCODE 3

WindowsLiveAction::WindowsLiveAction(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo)
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
	return _getStringFromResourceIDName(IDS_WINDOWSLIVE_DESCRIPTION, szName);
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
	if (_getMajorVersion() == MS_LIVE_ESSENTIALS_2009)
	{	
		ConfigurationFileActionDownload downloadVersion;
		wchar_t version[32];

		swprintf_s(version, L"%u", MS_LIVE_ESSENTIALS_2009);
		downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(version));
		GetTempPath(MAX_PATH, m_szFilename);
		wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

		return m_downloadManager->GetFile(downloadVersion, m_szFilename, progress, data);
	}

	// The installer for Essentials 2011 downloads the language packs. We indicate that the action
	// downloads, but it is delagated to the installer (as this the internet connection
	// detection is checked for this action)
	return true;
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

	// Live Essential 2009 parameters: http://www.mydigitallife.info/windows-live-essentials-unattended-silent-setup-installation-switches/
	if (_getMajorVersion() == MS_LIVE_ESSENTIALS_2009)
	{
		wcscpy_s(szApp, m_szFilename);
		// By selecting only Silverlight the installer will update only the installer apps
		// instead of installing also new ones. This saves us to dectect which exact component
		// is installed and uninstall it
		wcscat_s(szApp, L" /AppSelect:Silverlight /quiet");		
	}
	else
	{
		wcscpy_s(szApp, location.c_str());
		wcscat_s(szApp, L" -install -language:ca /quiet");
	}

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
	g_log.Log(L"WindowsLiveAction::_isLangSelected2011(). Language %s", szLanguage);
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

void WindowsLiveAction::CheckPrerequirements(Action * action)
{	
	_readVersionInstalled();

	if (m_version.size() > 0)
	{
		int majorVersion;

		if (_isLangSelected() == true)
		{
			SetStatus(AlreadyApplied);
			return;
		}
		
		majorVersion = _getMajorVersion();
		if (majorVersion != MS_LIVE_ESSENTIALS_2009 && majorVersion != MS_LIVE_ESSENTIALS_2011)
		{
			_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"WindowsLiveAction::CheckPrerequirements. Version not supported");
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
