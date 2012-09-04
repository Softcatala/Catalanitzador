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
#include "OSVersion.h"
#include "Runner.h"
#include "Url.h"
#include "Resources.h"
#include "ConfigurationInstance.h"
#include "WindowsValidation.h"

#include <sstream>

WindowsLPIAction::WindowsLPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
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
		case WindowsXP:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWSXP);
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
		case WindowsXP:
		{
			WORD majorVersion = HIWORD(m_OSVersion->GetServicePackVersion());

			// The Windows LPI for SP2 or higher checks if the the Windows is validated and fails if is not
			// If the version of Windows is not validated, we switch back to the old language pack that does not
			// do any validation
			if (majorVersion >= 2 && _isWindowsValidated())
			{
				return L"XP2";
			}
			else
			{
				return L"XP";
			}
		}

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
		case Windows8:
			if (m_OSVersion->IsWindows64Bits())
			{
				return L"Win8_64";
			}
			else
			{
				return L"Win8_32";
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
	if (m_OSVersion->GetVersion() == WindowsXP)
		return true;

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

	if (version == WindowsXP)
	{
		if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\", false))
		{
			wchar_t szValue[1024];

			// MultiUILanguageId key is left behind
			if (m_registry->GetString(L"MUILanguagePending", szValue, sizeof (szValue)))
			{
				if (wcsstr(szValue, L"0403") != NULL)
					bExists = true;
			}
			m_registry->Close();
		}
	}
	else  //(version == WindowsVista) or 7
	{
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
	}
	
	g_log.Log (L"WindowsLPIAction::_isLangPackInstalled returns %u", (wchar_t*) bExists);
	return bExists;
}

bool WindowsLPIAction::IsNeed()
{
 	if (status == CannotBeApplied)
		return false;
	
	bool bNeed = false;

	if (_getDownloadID() != NULL)
	{		
		if (_isLangPackInstalled() == false || _isDefaultLanguage() == false)
		{
			bNeed = true;
		}
		else
		{
			status = AlreadyApplied;
		}
	}
	else
	{
		SetStatus(CannotBeApplied);
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::IsNeed. Unsupported Windows version");
	}

	g_log.Log(L"WindowsLPIAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool WindowsLPIAction::Download(ProgressStatus progress, void *data)
{
	wstring filename;	
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFile(downloadVersion, m_szFilename, progress, data);
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

	OperatingVersion version = m_OSVersion->GetVersion();

	if (version == WindowsXP)
	{
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\msiexec.exe ");

		wcscpy_s(szParams, L" /i ");
		wcscat_s(szParams, m_szFilename); // full path to 'lip_ca-es.msi'
		wcscat_s(szParams, L" /qn");
	}
	else // Windows Vista and 7
	{	
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s(szParams, L" /i ca-ES /r /s /p ");
		wcscat_s(szParams, m_szFilename);
	
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\lpksetup.exe");
	}

	status = InProgress;
	g_log.Log(L"WindowsLPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
	m_runner->Execute(lpkapp, szParams, m_OSVersion->IsWindows64Bits());
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void WindowsLPIAction::_setDefaultLanguage()
{
	if (m_OSVersion->GetVersion() == WindowsXP)
		return;
	
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

#define SCRIPT_NAME L"lang.ps1"

void WindowsLPIAction::_setLanguagePanelWin8()
{
	if (m_OSVersion->GetVersion() != Windows8)
		return;

	string script;
	script = "$1 = New-WinUserLanguageList ca\r\n";
	script += "$1 += Get-WinUserLanguageList\r\n"; 
	script += "Set-WinUserLanguageList $1 -Force\r\n";
	
	wchar_t szScript[MAX_PATH];

	GetTempPath(MAX_PATH, szScript);
	wcscat_s(szScript, SCRIPT_NAME);

	ofstream of(szScript);
	of.write(script.c_str(), script.size());
	of.close();
	
	Runner runner;
	wstring params;
	wchar_t szTool[MAX_PATH];

	GetSystemDirectory(szTool, MAX_PATH);
	wcscat_s(szTool, L"\\WindowsPowerShell\\v1.0\\powershell.exe");

	params = L" -ExecutionPolicy remotesigned ";
	params+= szScript;

	runner.Execute(szTool, (wchar_t *)params.c_str());
	g_log.Log(L"WindowsLPIAction::_setLanguagePanelWin8 '%s' with params '%s'", szTool,	(wchar_t *) params.c_str());
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
			_setLanguagePanelWin8();
			_setDefaultLanguage();
		}
		else
		{
			SetStatus(FinishedWithError);
		}
		
		g_log.Log(L"WindowsLPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool WindowsLPIAction::IsRebootNeed() const
{
	if (m_OSVersion->GetVersion() == Windows8)
		return false;

	return status == Successful;
}

#define SPANISH_LOCALEID 0x0C0A
#define FRENCH_LOCALEID 0x040C

bool WindowsLPIAction::_isASupportedSystemLanguage()
{
	bool bLangOk = false;

	if (m_OSVersion->GetVersion() == Windows8)
		return true;
	
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		LANGID langid;
		WORD primary;

		langid = m_win32I18N->GetSystemDefaultUILanguage();
		primary = PRIMARYLANGID(langid);

		bLangOk = (primary == PRIMARYLANGID(SPANISH_LOCALEID));
		g_log.Log(L"WindowsLPIAction::_isASupportedSystemLanguage. Language ID: %x", (wchar_t* )langid);
	}
	else
	{
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
	}
	return bLangOk;
}

void WindowsLPIAction::CheckPrerequirements(Action * action)
{
	if (m_OSVersion->IsWindows64Bits() == false)
	{
		if (m_OSVersion->GetVersion() != WindowsXP
			&& m_OSVersion->GetVersion() != WindowsVista
			&& m_OSVersion->GetVersion() != Windows7
			&& m_OSVersion->GetVersion() != Windows8)
		{
			_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDWIN, szCannotBeApplied);
			g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported Windows version");
			SetStatus(CannotBeApplied);
			return;
		}
	} 
	else // 64 bits
	{
		if (m_OSVersion->GetVersion() != Windows7 && m_OSVersion->GetVersion() != Windows8)
		{
			_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDWIN, szCannotBeApplied);
			g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported Windows version");
			SetStatus(CannotBeApplied);
			return;
		}
	}

	if (_isASupportedSystemLanguage() == false)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NOSPFR, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported base language");
		SetStatus(CannotBeApplied);
		return;
	}
}

bool WindowsLPIAction::_isWindowsValidated()
{
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		DWORD dwservicePack;

		dwservicePack = m_OSVersion->GetServicePackVersion();
	
		if (HIWORD(dwservicePack) < 2)
		{
			g_log.Log(L"IsWindowsValidated::IsWindowsValidated. Old XP");
			return true;
		}
		 
		return WindowsValidation::IsWindowsValidated();
	}
	else
	{
		g_log.Log(L"IsWindowsValidated::IsWindowsValidated. No XP");
		return true;
	}
}

