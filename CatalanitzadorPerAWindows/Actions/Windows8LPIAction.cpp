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

#include "Windows8LPIAction.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Url.h"
#include "Resources.h"
#include "ConfigurationInstance.h"
#include "WindowsValidation.h"

#include <sstream>

Windows8LPIAction::Windows8LPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
{
	m_registry = registry;
	m_win32I18N = win32I18N;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_szFilename[0] = NULL;
}

Windows8LPIAction::~Windows8LPIAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

wchar_t* Windows8LPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NAME, szName);
}

wchar_t* Windows8LPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_DESCRIPTION, szDescription);
}

LPCWSTR Windows8LPIAction::GetLicenseID()
{
	// If the package is installed, no need to accept the license
	if (_isLangPackInstalled() == true)
		return NULL;
	
	return MAKEINTRESOURCE(IDR_LICENSE_WINDOWS8);
}

wchar_t* Windows8LPIAction::_getDownloadID()
{
	if (m_OSVersion->IsWindows64Bits())
	{
		return L"Win8_64";
	}
	else
	{
		return L"Win8_32";
	}
}

#define LANGUAGE_CODE L"ca-ES"

bool Windows8LPIAction::IsDownloadNeed()
{
	return _isLangPackInstalled() == false;
}


// Checks if the Catalan language pack is already installed
// This code works if the langpack is installed or has just been installed (and the user did not reboot)
bool Windows8LPIAction::_isLangPackInstalled()
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
	
	g_log.Log (L"Windows8LPIAction::_isLangPackInstalled returns %u", (wchar_t*) bExists);
	return bExists;
}

bool Windows8LPIAction::IsNeed()
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
	g_log.Log(L"Windows8LPIAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

bool Windows8LPIAction::Download(ProgressStatus progress, void *data)
{
	wstring filename;	
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFile(downloadVersion, m_szFilename, progress, data);
}

void Windows8LPIAction::Execute()
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];
	
	if (_isLangPackInstalled() == false)
	{
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s(szParams, L" /i ca-ES /r /s /p ");
		wcscat_s(szParams, m_szFilename);

		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\lpksetup.exe");
		
		g_log.Log(L"Windows8LPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
		m_runner->Execute(lpkapp, szParams, m_OSVersion->IsWindows64Bits());
	}

	SetStatus(InProgress);
}

#define SCRIPT_NAME L"lang.ps1"

void Windows8LPIAction::_setLanguagePanelWin8()
{
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
	g_log.Log(L"Windows8LPIAction::_setLanguagePanelWin8 '%s' with params '%s'", szTool, (wchar_t *) params.c_str());
	runner.WaitUntilFinished();
	DeleteFile(szScript);
}

bool Windows8LPIAction::_isAlreadyApplied()
{
	bool panel, langpack, deflang;

	panel = _isLanguagePanelWin8First();
	langpack = _isLangPackInstalled();
	deflang = _isDefaultLanguage();

	return panel && langpack && deflang;
}

bool Windows8LPIAction::_isLanguagePanelWin8First()
{
	bool bRslt;
	wstring langcode, firstlang;

	// TODO: Read always return a single language
	_readLanguageCode(langcode);
	//_parseLanguage(langcode);
	_getFirstLanguage(firstlang);

	bRslt = firstlang.compare(L"ca") == 0;
	g_log.Log(L"Windows8LPIAction::_isLanguagePanelWin8First '%u' (%s)", (wchar_t *) bRslt, (wchar_t *) firstlang.c_str());
	return bRslt;
}

void Windows8LPIAction::_getFirstLanguage(wstring& regvalue)
{
	if (m_languages.size() > 0)
	{		
		regvalue = m_languages[0];
		std::transform(regvalue.begin(), regvalue.end(), regvalue.begin(), ::tolower);
		return;
	}
	
	regvalue.clear();
	return;
}

void Windows8LPIAction::_parseLanguage(wstring regvalue)
{
	wstring language;
	
	m_languages.clear();
	for (unsigned int i = 0; i < regvalue.size (); i++)
	{
		if (regvalue[i] == L' ')
		{
			m_languages.push_back(language);
			language.clear();
			continue;
		}

		language += regvalue[i];
	}

	if (language.empty() == false)
	{
		m_languages.push_back(language);
	}
}

void Windows8LPIAction::_readLanguageCode(wstring& languages)
{
	wchar_t szValue[1024];

	m_languages.clear();
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\International\\User Profile", false))
	{
		if (m_registry->GetString(L"Languages", szValue, sizeof (szValue)))
		{
			
		}
		// TODO: Temp. Also GetString when multiple langs returns error and reads the first one
		languages = szValue;

		if (languages.size() > 0)
		{
			m_languages.push_back(szValue);
			g_log.Log(L"Windows8LPIAction::_readLanguageCode is '%s'", szValue);
		}
		m_registry->Close();
	}
}


// The langpack may be installed but not selected
bool Windows8LPIAction::_isDefaultLanguage()
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

	g_log.Log(L"WindowsLPIAction::_isDefaultLanguage preferred lang '%s', preferred pending lang '%s', machine preferred '%s'",
		szPreferred, szPreferredPending, szPreferredMachine);

	return (_wcsnicmp(szPreferred, LANGUAGE_CODE, langCodeLen) == 0) ||
		(_wcsnicmp(szPreferredPending, LANGUAGE_CODE, langCodeLen) == 0) ||
		(_wcsnicmp(szPreferredMachine, LANGUAGE_CODE, langCodeLen) == 0);
}

ActionStatus Windows8LPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		
		if (_isLanguagePanelWin8First() == false)
		{
			_setLanguagePanelWin8();
		}

		_setDefaultLanguage();
		
		if (_isAlreadyApplied())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
		}
		
		g_log.Log(L"Windows8LPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool Windows8LPIAction::IsRebootNeed() const
{
	return status == Successful;
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void Windows8LPIAction::_setDefaultLanguage()
{
	// Sets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", true) == TRUE)
	{
		m_registry->SetString(L"PreferredUILanguages", L"ca-ES");
		// Needed since the panel forces it to other languages
		m_registry->SetMultiString(L"PreferredUILanguagesPending", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"Windows8LPIAction::_setDefaultLanguage current user done");
	}

	// Sets the language for all users
	/*if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\Settings", true) == TRUE)
	{
		m_registry->SetMultiString(L"PreferredUILanguages", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"Windows8LPIAction::_setDefaultLanguage all users done");
	}*/
}

void Windows8LPIAction::CheckPrerequirements(Action * action)
{
	if (_getDownloadID() == NULL)
	{
		SetStatus(CannotBeApplied);
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDWIN, szCannotBeApplied);
		g_log.Log(L"Windows8LPIAction::IsNeed. Unsupported Windows version");
		return;
	}

	if (_isAlreadyApplied())
	{
		SetStatus(AlreadyApplied);
	}
}
