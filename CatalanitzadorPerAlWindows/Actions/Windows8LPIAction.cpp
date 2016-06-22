/* 
 * Copyright (C) 2012-2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Runner.h"
#include "ConfigurationInstance.h"
#include "StringConversion.h"

#define VALENCIAN_PANEL_LANGCODE L"ca-es-valencia"
#define CATALAN_PANEL_LANGCODE L"ca"

#define VALENCIAN_LANGPACKCODE L"ca-es-valencia"
#define CATALAN_LANGPACKCODE L"ca-ES"

#define LANGUAGE_CODE L"ca-ES"
#define SCRIPT_NAME L"lang.ps1"

Windows8LPIAction::Windows8LPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, DownloadManager* downloadManager):
WindowsLPIBaseAction(downloadManager)
{
	m_registry = registry;
	m_win32I18N = win32I18N;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_executionStep = ExecutionStepNone;
}

Windows8LPIAction::~Windows8LPIAction()
{
	if (m_filename.empty() == false  && GetFileAttributes(m_filename.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename.c_str());
	}

	if (m_scriptfile.empty() == false  && GetFileAttributes(m_scriptfile.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_scriptfile.c_str());
	}
}

wchar_t* Windows8LPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_WINDOWS8LPIACTION_NAME, szName);
}

wchar_t* Windows8LPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWS8LPIACTION_DESCRIPTION, szDescription);
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
	_selectLanguagePackage();
	return (wchar_t*) m_packageDownloadId.c_str();
}


//Windows 10's November 2015 upgrade, build 10586
#define BUILD_10586 10586

void Windows8LPIAction::_selectLanguagePackageW10()
{
	DWORD buildNumber = m_OSVersion->GetBuildNumber();

	if (buildNumber < BUILD_10586)
	{
		if (m_OSVersion->IsWindows64Bits())
		{
			if (GetUseDialectalVariant())
			{
				m_packageDownloadId = L"Win10_va_64";
				m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
			}
			else
			{
				m_packageDownloadId = L"Win10_ca_64";
				m_packageLanguageCode = CATALAN_LANGPACKCODE;
			}
		}
		else
		{
			if (GetUseDialectalVariant())
			{
				m_packageDownloadId = L"Win10_va_32";
				m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
			}
			else
			{
				m_packageDownloadId = L"Win10_ca_32";
				m_packageLanguageCode = CATALAN_LANGPACKCODE;
			}
		}
	}

	if (buildNumber >= BUILD_10586)
	{
		// TODO: Add Valencian langpacks when they are published
		if (m_OSVersion->IsWindows64Bits())
		{
			m_packageDownloadId = L"Win10_10586_ca_64";
			m_packageLanguageCode = CATALAN_LANGPACKCODE;
		}
		else
		{
			m_packageDownloadId = L"Win10_10586_ca_32";
			m_packageLanguageCode = CATALAN_LANGPACKCODE;
		}
	}
}

void Windows8LPIAction::_selectLanguagePackage()
{
	OperatingVersion version = m_OSVersion->GetVersion();	

	switch (version)
	{
		case Windows10:
		{
			_selectLanguagePackageW10();	
			break;
		}

		case Windows81:
		{
			if (m_OSVersion->IsWindows64Bits())
			{
				if (GetUseDialectalVariant())
				{
					m_packageDownloadId = L"Win81_va_64";
					m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
				}
				else
				{
					m_packageDownloadId = L"Win81_ca_64";
					m_packageLanguageCode = CATALAN_LANGPACKCODE;
				}
			}
			else
			{
				if (GetUseDialectalVariant())
				{
					m_packageDownloadId = L"Win81_va_32";
					m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
				}
				else
				{
					m_packageDownloadId = L"Win81_ca_32";
					m_packageLanguageCode = CATALAN_LANGPACKCODE;
				}				
			}
			break;
		}

		case Windows8:
		{
			if (m_OSVersion->IsWindows64Bits())
			{
				if (GetUseDialectalVariant())
				{
					m_packageDownloadId = L"Win8_va_64";
					m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
				}
				else
				{
					m_packageDownloadId = L"Win8_ca_64";
					m_packageLanguageCode = CATALAN_LANGPACKCODE;
				}
			}
			else
			{
				if (GetUseDialectalVariant())
				{
					m_packageDownloadId = L"Win8_va_32";
					m_packageLanguageCode = VALENCIAN_LANGPACKCODE;
				}
				else
				{
					m_packageDownloadId = L"Win8_ca_32";
					m_packageLanguageCode = CATALAN_LANGPACKCODE;
				}
			}
			break;
		}
		default:
			m_packageLanguageCode.clear();
			m_packageDownloadId.clear();
	}
}

bool Windows8LPIAction::IsDownloadNeed()
{
	return _isLangPackInstalled() == false;
}

// Checks if the Catalan language pack is already installed
// This code works if the langpack is installed or has just been installed (and the user did not reboot)
bool Windows8LPIAction::_isLangPackInstalledForLanguage(wstring langcode)
{
	bool bExists = false;
	wstring key;

	key = L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\";
	key += langcode;
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t *) key.c_str(), false))
	{
		bExists = true;
		m_registry->Close();
	}
	// If you install updates without rebooting, and then the language pack it gets registered in PendingInstall and not in the UILanguages key
	else 
	{
		key = L"SYSTEM\\CurrentControlSet\\Control\\MUI\\PendingInstall\\";
		key += langcode;
		if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t *) key.c_str(), false))
		{
			bExists = true;
			m_registry->Close();
		}
	}
	
	g_log.Log (L"Windows8LPIAction::_isLangPackInstalledForLanguage '%s' returns %u", (wchar_t*)langcode.c_str(), (wchar_t*) bExists);
	return bExists;
}

bool Windows8LPIAction::_isLangPackInstalled()
{
	return _isLangPackInstalledForLanguage(CATALAN_LANGPACKCODE) ||
		_isLangPackInstalledForLanguage(VALENCIAN_LANGPACKCODE);
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
	wchar_t filename[MAX_PATH];
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));
	GetTempPath(MAX_PATH, filename);
	wcscat_s(filename, downloadVersion.GetFilename().c_str());
	m_filename = filename;

	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, filename, progress, data);
}

void Windows8LPIAction::Execute()
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];

	if (_isLangPackInstalled() == false)
	{
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s(szParams, L" /i ");
		wcscat_s(szParams, m_packageLanguageCode.c_str());
		wcscat_s(szParams, L" /r /s /p ");

		wcscat_s(szParams, m_filename.c_str());

		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\lpksetup.exe");
		
		g_log.Log(L"Windows8LPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
		m_runner->Execute(lpkapp, szParams, m_OSVersion->IsWindows64Bits());
	}

	m_executionStep = ExecutionStepProgram;
	SetStatus(InProgress);
}

void Windows8LPIAction::_buildLanguagePanelPowerShellScript(const wstring primaryCode, const wstring secondaryCode, string& script)
{
	string langcodeAnsi;

	StringConversion::ToMultiByte(primaryCode, langcodeAnsi);
	
	script = "$1 = New-WinUserLanguageList ";
	script += langcodeAnsi;
	script += "\r\n";

	if (secondaryCode.empty() == false)
	{
		StringConversion::ToMultiByte(secondaryCode, langcodeAnsi);
		script += "$1 += \"";
		script += langcodeAnsi;
		script += "\"\r\n";
	}

	script += "$1 += Get-WinUserLanguageList\r\n"; 
	script += "Set-WinUserLanguageList $1 -Force\r\n";
}

void Windows8LPIAction::_runLanguagePanelPowerShellScript(const string script)
{
	Runner runner;
	wstring params;
	wchar_t szTool[MAX_PATH];

	assert(m_scriptfile.size() > 0);
	
	GetSystemDirectory(szTool, MAX_PATH);
	wcscat_s(szTool, L"\\WindowsPowerShell\\v1.0\\powershell.exe");

	params = L" -ExecutionPolicy remotesigned ";
	params+= m_scriptfile.c_str();
	
	g_log.Log(L"Windows8LPIAction::_runLanguagePanelPowerShellScript '%s' with params '%s'", szTool, (wchar_t *)params.c_str());
	m_runner->Execute(szTool, (wchar_t *)params.c_str(), m_OSVersion->IsWindows64Bits());
}

void Windows8LPIAction::_setLanguagePanelLanguages(const wstring primaryCode, const wstring secondaryCode)
{
	wchar_t szScript[MAX_PATH];
	string script;

	GetTempPath(MAX_PATH, szScript);
	wcscat_s(szScript, SCRIPT_NAME);
	m_scriptfile = szScript;

	_buildLanguagePanelPowerShellScript(primaryCode, secondaryCode, script);

	ofstream of(szScript);
	of.write(script.c_str(), script.size());
	of.close();
	
	_runLanguagePanelPowerShellScript(script);

	g_log.Log(L"Windows8LPIAction::_setLanguagePanelLanguages. Langs: '%s', '%s'", (wchar_t *) primaryCode.c_str(),
		(wchar_t *) secondaryCode.c_str());
}

void Windows8LPIAction::_setLanguagePanel()
{
	if (GetUseDialectalVariant())
	{
		_setLanguagePanelLanguages(VALENCIAN_PANEL_LANGCODE, CATALAN_PANEL_LANGCODE);
	}
	else
	{
		_setLanguagePanelLanguages(CATALAN_PANEL_LANGCODE, wstring());
	}
}

bool Windows8LPIAction::_isAlreadyApplied()
{
	bool panel, langpack, deflang;

	panel = _isLanguagePanelFirst();
	langpack = _isLangPackInstalled();
	deflang = _isDefaultLanguage();

	return panel && langpack && deflang;
}

bool Windows8LPIAction::_isLanguagePanelFirstForLanguage(wstring expectedcode)
{
	bool bRslt;
	wstring langcode;

	_readPanelLanguageCode(langcode);
	
	std::transform(langcode.begin(), langcode.end(), langcode.begin(), ::tolower);
	bRslt = langcode.compare(expectedcode) == 0;
	g_log.Log(L"Windows8LPIAction::_isLanguagePanelFirstForLanguage '%u' (%s)", (wchar_t *) bRslt, (wchar_t *) langcode.c_str());
	return bRslt;
}

bool Windows8LPIAction::_isLanguagePanelFirst()
{
	return _isLanguagePanelFirstForLanguage(CATALAN_PANEL_LANGCODE) ||
		_isLanguagePanelFirstForLanguage(VALENCIAN_PANEL_LANGCODE);
}

void Windows8LPIAction::_readPanelLanguageCode(wstring& language)
{
	wchar_t szValue[1024] = L"";
	
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\International\\User Profile", false))
	{
		// Languages is a registry key of type REG_MULTI_SZ. As such can have serveral strings.
		// By using GetString (instead of a MULTI_SZ read) we only read the first one and the call returns error
		// This is OK since we just need the first language
		m_registry->GetString(L"Languages", szValue, sizeof (szValue));
		language = szValue;

		if (language.size() > 0)
		{			
			g_log.Log(L"Windows8LPIAction::_readPanelLanguageCode is '%s'", szValue);
		}
		m_registry->Close();
	}
}

// The langpack may be installed but not selected
bool Windows8LPIAction::_isDefaultLanguageForLanguage(wstring langcode)
{
	wchar_t szPreferred[2048] =L"";	
	wchar_t szPreferredPending[2048] =L"";	
	const int langCodeLen = langcode.size();
	
	// Gets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", false))
	{
		m_registry->GetString(L"PreferredUILanguages", szPreferred, sizeof(szPreferred));
		m_registry->GetString(L"PreferredUILanguagesPending", szPreferredPending, sizeof(szPreferredPending));
		m_registry->Close();
	}

	g_log.Log(L"Windows8LPIAction::_isDefaultLanguageForLanguage preferred lang '%s', preferred pending lang '%s'", 
		szPreferred, szPreferredPending);

	return (_wcsnicmp(szPreferred, langcode.c_str(), langCodeLen) == 0) ||
		(_wcsnicmp(szPreferredPending, langcode.c_str(), langCodeLen) == 0);
}

bool Windows8LPIAction::_isDefaultLanguage()
{
	return _isDefaultLanguageForLanguage(CATALAN_LANGPACKCODE) ||
		_isDefaultLanguageForLanguage(VALENCIAN_LANGPACKCODE);
}

ActionStatus Windows8LPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		switch (m_executionStep)
		{
			case ExecutionStepNone:
				break;
			case ExecutionStepProgram:
				if (_isLanguagePanelFirst() == false)
				{
					_setLanguagePanel();
					m_executionStep = ExecutionStepCfgLocale;
					return InProgress;
				}
			case ExecutionStepCfgLocale:
				break;
			default:
				assert(false);
				break;
		}

		_setDefaultLanguage();
		
		if (_isAlreadyApplied())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
			_dumpLpkSetupErrors();
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
		wchar_t* packageLanguageCode = (wchar_t *) m_packageLanguageCode.c_str();

		m_registry->SetString(L"PreferredUILanguages", packageLanguageCode);
		// Needed since the panel forces it to other languages
		m_registry->SetMultiString(L"PreferredUILanguagesPending", packageLanguageCode);
		m_registry->Close();
		g_log.Log(L"Windows8LPIAction::_setDefaultLanguage current user done: %s", packageLanguageCode);
	}

	// Sets the preferred language for all users. Impacts:
	//   1. When we logging, this settings helps to show the "Welcome" message in the localized language
	//   2. Sets the installed language as default for all users
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\Settings", true) == TRUE)
	{
		wchar_t* packageLanguageCode = (wchar_t *) m_packageLanguageCode.c_str();

		m_registry->SetMultiString(L"PreferredUILanguages", packageLanguageCode);
		m_registry->Close();
		g_log.Log(L"Windows8LPIAction::_setDefaultLanguage all users to %s done", packageLanguageCode);
	}
}


#define SPANISH_LOCALEID 0x0C0A
#define FRENCH_LOCALEID 0x040C
#define US_LOCALEID 0x0409
#define UK_LOCALEID 0x0809

bool Windows8LPIAction::_isASupportedSystemLanguage()
{
	bool bLangOk = false;

	vector <LANGID> langIDs;
	langIDs = m_win32I18N->EnumUILanguages();

	for (unsigned int i = 0; i < langIDs.size(); i++)
	{
		if (langIDs.at(i) == SPANISH_LOCALEID ||
			langIDs.at(i) == FRENCH_LOCALEID ||
			langIDs.at(i) == US_LOCALEID ||
			langIDs.at(i) == UK_LOCALEID)
		{				
			bLangOk = true;			
		}
		g_log.Log(L"Windows8LPIAction::_isASupportedSystemLanguage. Language ID: %x", (wchar_t* )langIDs.at(i));
	}
	
	return bLangOk;
}

void Windows8LPIAction::CheckPrerequirements(Action * action)
{
	if (_isASupportedSystemLanguage() == false)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NOSPFR, szCannotBeApplied);
		g_log.Log(L"Windows8LPIAction::CheckPrerequirements. Unsupported base language");
		SetStatus(CannotBeApplied);
		return;
	}

	if (_isAlreadyApplied())
	{
		SetStatus(AlreadyApplied);
		return;
	}

	ConfigurationFileActionDownload downloadVersion;
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));
	if (downloadVersion.IsUsable() == false)
	{
		_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"Windows8LPIAction::CheckPrerequirements. Version not supported");
		SetStatus(CannotBeApplied);
		return;
	}
}
