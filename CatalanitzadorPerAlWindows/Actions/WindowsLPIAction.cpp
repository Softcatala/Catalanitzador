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

#include "WindowsLPIAction.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

WindowsLPIAction::WindowsLPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, DownloadManager* downloadManager)
: WindowsLPIBaseAction(downloadManager)
{
	m_registry = registry;
	m_win32I18N = win32I18N;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_szFilename[0] = NULL;
}

WindowsLPIAction::~WindowsLPIAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}

	if (m_msiexecLog.empty() == false && GetFileAttributes(m_msiexecLog.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_msiexecLog.c_str());
	}
}

wchar_t* WindowsLPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NAME, szName);
}

wchar_t* WindowsLPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_DESCRIPTION, szDescription);
}

LPCWSTR WindowsLPIAction::GetLicenseID()
{
	OperatingVersion version = m_OSVersion->GetVersion();

	switch (version)
	{
		case WindowsVista:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWSVISTA);
		case Windows7:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWS7);
		default:
			break;
	}
	return NULL;
}

wchar_t* WindowsLPIAction::_getDownloadID()
{
	OperatingVersion version = m_OSVersion->GetVersion();

	switch (version)
	{
		case WindowsVista:
			return L"Vista";
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return L"Win7_64";
			}
			else
			{
				return L"Win7_32";
			}
		default:
			break;
	}
	return NULL;
}

#define LANGUAGE_CODE L"ca-ES"

bool WindowsLPIAction::IsDownloadNeed()
{
	return _isLangPackInstalled() == false;
}

// The langpack may be installed but not selected
bool WindowsLPIAction::_isDefaultLanguage()
{
	wchar_t szPreferred[2048] =L"";	
	wchar_t szPreferredPending[2048] =L"";
	wchar_t szPreferredMachine[2048] =L"";
	const int langCodeLen = wcslen(LANGUAGE_CODE);
	
	// Gets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", false))
	{
		m_registry->GetString(L"PreferredUILanguages", szPreferred, sizeof(szPreferred));
		m_registry->GetString(L"PreferredUILanguagesPending", szPreferredPending, sizeof(szPreferredPending));
		m_registry->Close();
	}

	// In Windows 7, created new users do not have preferences and use the one reflected on MachinePreferredUILanguages
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\MuiCached", false))
	{
		m_registry->GetString(L"MachinePreferredUILanguages", szPreferredMachine, sizeof(szPreferredMachine));
		m_registry->Close();
	}

	g_log.Log(L"WindowsLPIAction::_isDefaultLanguage preferred lang '%s', preferred pending lang '%s', machine preferred '%s'",
		szPreferred, szPreferredPending, szPreferredMachine);

	return (_wcsnicmp(szPreferred, LANGUAGE_CODE, langCodeLen) == 0) ||
		(_wcsnicmp(szPreferredPending, LANGUAGE_CODE, langCodeLen) == 0) ||
		(_wcsnicmp(szPreferredMachine, LANGUAGE_CODE, langCodeLen) == 0);
}

// Checks if the Catalan language pack is already installed
// This code works if the langpack is installed or has just been installed (and the user did not reboot)
bool WindowsLPIAction::_isLangPackInstalled()
{	
	bool bExists = false;
	OperatingVersion version = m_OSVersion->GetVersion();	
	
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES", false))
	{
		bExists = true;
		m_registry->Close();
	}
	// If you install updates without rebooting, and then the language pack it gets registered in PendingInstall and not in the UILanguages key
	else if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\ca-ES", false))
	{
		bExists = true;
		m_registry->Close();
	}
	
	g_log.Log (L"WindowsLPIAction::_isLangPackInstalled returns %u", (wchar_t*) bExists);
	return bExists;
}

bool WindowsLPIAction::IsNeed()
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
	g_log.Log(L"WindowsLPIAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;	
}

bool WindowsLPIAction::Download(ProgressStatus progress, void *data)
{
	wstring filename;	
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}

void WindowsLPIAction::Execute()
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];

	if (_isLangPackInstalled() == true)
	{
		_setDefaultLanguage();
		status = _isLangPackInstalled() ? Successful : FinishedWithError;
		g_log.Log(L"WindowsLPIAction::Execute. Setting default language only was '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
		return;
	}	

	// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
	wcscpy_s(szParams, L" /i ca-ES /r /s /p ");
	wcscat_s(szParams, m_szFilename);

	GetSystemDirectory(lpkapp, MAX_PATH);
	wcscat_s(lpkapp, L"\\lpksetup.exe");	

	status = InProgress;
	g_log.Log(L"WindowsLPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
	m_runner->Execute(lpkapp, szParams, m_OSVersion->IsWindows64Bits());
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void WindowsLPIAction::_setDefaultLanguage()
{	
	// Sets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", true) == TRUE)
	{
		m_registry->SetString(L"PreferredUILanguages", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"WindowsLPIAction::_setDefaultLanguage current user done");
	}

	// Sets the language for all users
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\Settings", true) == TRUE)
	{
		m_registry->SetMultiString(L"PreferredUILanguages", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"WindowsLPIAction::_setDefaultLanguage all users done");
	}
}

ActionStatus WindowsLPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangPackInstalled()) 
		{
			status = Successful;
			_setDefaultLanguage();
		}
		else
		{
			SetStatus(FinishedWithError);			
			_dumpLpkSetupErrors();
		}
		
		g_log.Log(L"WindowsLPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool WindowsLPIAction::IsRebootNeed() const
{
	return status == Successful;
}

#define SPANISH_LOCALEID 0x0C0A
#define FRENCH_LOCALEID 0x040C

bool WindowsLPIAction::_isASupportedSystemLanguage()
{
	bool bLangOk = false;
	vector <LANGID> langIDs;
	langIDs = m_win32I18N->EnumUILanguages();

	for (unsigned int i = 0; i < langIDs.size(); i++)
	{
		if (langIDs.at(i) == SPANISH_LOCALEID || langIDs.at(i) == FRENCH_LOCALEID)
		{				
			bLangOk = true;
		}
		g_log.Log(L"WindowsLPIAction::_isASupportedSystemLanguage. Language ID: %x", (wchar_t* )langIDs.at(i));
	}	
	return bLangOk;
}

bool WindowsLPIAction::_isValidOperatingSystem()
{
	if (m_OSVersion->IsWindows64Bits() == false)
	{
		if (m_OSVersion->GetVersion() != WindowsVista
			&& m_OSVersion->GetVersion() != Windows7)
		{
			return false;
		}
	} 
	else // 64 bits
	{
		if (m_OSVersion->GetVersion() != Windows7)
		{
			return false;
		}
	}
	return true;
}

void WindowsLPIAction::CheckPrerequirements(Action * action)
{
	if (_isValidOperatingSystem() == false)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported Windows version");
		SetStatus(CannotBeApplied);
		return;
	}

	if (_isASupportedSystemLanguage() == false)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NOSPFR, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported base language");
		SetStatus(CannotBeApplied);
		return;
	}

	if (_isDownloadAvailable())
	{
		if ((_isLangPackInstalled() && _isDefaultLanguage()))
		{
			SetStatus(AlreadyApplied);			
		}
	}
	else
	{
		SetStatus(CannotBeApplied);
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::IsNeed. Unsupported Windows version");
	}
}

bool WindowsLPIAction::_isDownloadAvailable()
{
	wstring filename;	
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));

	return downloadVersion.IsUsable();
}
