/* 
 * Copyright (C) 2012 Xavier Ivars-Ribes <xavi.ivars@gmail.com>
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
#include "ChromeAction.h"
#include "Runner.h"

#ifndef CHROME_LANGUAGECODE
#define CHROME_LANGUAGECODE L"ca"
#endif

ChromeAction::ChromeAction(IRegistry* registry)
{
	m_chromeProfile = NULL;
	m_registry = registry;

	_addExecutionProcess(ExecutionProcess(L"chrome.exe", L"", true));
	
	SetChromeProfile(new ChromeProfile());
	m_allocatedProfile = true;
	
}

ChromeAction::~ChromeAction()
{
	SetChromeProfile(NULL);
}

wchar_t* ChromeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_NAME, szName);
}

wchar_t* ChromeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_CHROMEACTION_DESCRIPTION, szDescription);	
}

void ChromeAction::FinishExecution(ExecutionProcess process)
{
	if (_getProcessIDs(process.GetName()).size() > 0)
	{
		Runner runner;
		runner.RequestCloseToProcessID(_getProcessIDs(process.GetName()).at(0), true);
	}
}

void ChromeAction::SetChromeProfile(ChromeProfile *profile)
{
	if (m_allocatedProfile && m_chromeProfile)
	{
		delete m_chromeProfile;
		m_allocatedProfile = false;
	}
	m_chromeProfile = profile;
}

bool ChromeAction::IsNeed()
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
	g_log.Log(L"ChromeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

void ChromeAction::Execute()
{
	SetStatus(InProgress);
	bool writeSpellAndAcceptLanguages = false;

	if (m_chromeProfile->IsUiLocaleOk() == false) {
		m_chromeProfile->WriteUILocale();
	}

	if (m_chromeProfile->IsAcceptLanguagesOk() == false) {
		m_chromeProfile->SetCatalanAsAcceptLanguages();
		writeSpellAndAcceptLanguages = true;
	}

	if (m_chromeProfile->IsSpellCheckerLanguageOk() == false) {
		m_chromeProfile->SetCatalanAsSpellCheckerLanguage();
		writeSpellAndAcceptLanguages = true;
	}

	if (writeSpellAndAcceptLanguages)
	{
		m_chromeProfile->WriteSpellAndAcceptLanguages();
	}
	
	if (m_chromeProfile->IsUiLocaleOk() && m_chromeProfile->IsAcceptLanguagesOk() && m_chromeProfile->IsSpellCheckerLanguageOk()) 
	{
		SetStatus(Successful);
	} 
	else 
	{
		SetStatus(FinishedWithError);
	}
	g_log.Log(L"ChromeAction::Execute returns %s", status == Successful ? L"Successful" : L"FinishedWithError");
}

void ChromeAction::_readVersion()
{
	if (m_registry->OpenKey(HKEY_CURRENT_USER, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			m_version = szVersion;
			g_log.Log(L"ChromeAction::_readVersion. Chrome version %s", szVersion);
		}
		m_registry->Close();
	} else if(m_registry->OpenKey(HKEY_LOCAL_MACHINE, CHROME_REGISTRY_PATH, false)) {
		wchar_t szVersion[1024];

		if (m_registry->GetString(L"Version", szVersion, sizeof(szVersion)))
		{
			m_version = szVersion;
			g_log.Log(L"ChromeAction::_readVersion. Chrome version %s", szVersion);
		}
		m_registry->Close();
	} 
}

bool ChromeAction::_isInstalled()
{
	if (m_isInstalled.IsUndefined())
	{
		wstring path;

		_readInstallLocation(path);
		m_isInstalled = path.size() > 0;
	}

	return m_isInstalled == true;
}

void ChromeAction::_readInstallLocation(wstring & path)
{
	path.erase();

	if(_findUserInstallation(path))
	{
		g_log.Log(L"ChromeAction::_readInstallLocation - %s",(wchar_t*) path.c_str());
	}
	else if(_findSystemInstallation(path))
	{
		g_log.Log(L"ChromeAction::_readInstallLocation - %s",(wchar_t*) path.c_str());
	}
	else 
	{
		g_log.Log(L"ChromeAction::_readInstallLocation - Chrome is not installed");
	}
}

bool ChromeAction::_findUserInstallation(wstring & path)
{
	bool isInstalled = false;
	if (m_registry->OpenKey(HKEY_CURRENT_USER, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szLocation[MAX_PATH];
		
		if (m_registry->GetString(L"InstallLocation", szLocation, sizeof(szLocation)))
		{
			path = szLocation;
			
			m_chromeProfile->SetPath(path);
			g_log.Log(L"ChromeAction::_findUserInstallation. InstallLocation %s", szLocation);
			isInstalled = true;
			
		}		
		m_registry->Close();
	}

	return isInstalled;
}

bool ChromeAction::_findSystemInstallation(wstring & path)
{
	bool isInstalled = false;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szLocation[MAX_PATH];
		
		if (m_registry->GetString(L"InstallLocation", szLocation, sizeof(szLocation)))
		{
			g_log.Log(L"ChromeAction::_findSystemInstallation. InstallLocation %s", szLocation);
			path = _getProfileRootDir();
			
			m_chromeProfile->SetPath(path);
			g_log.Log(L"ChromeAction::_findSystemInstallation. Profile %s", (wchar_t*) path.c_str());
			isInstalled = true;
		}		
		m_registry->Close();
	}

	return isInstalled;
}

wstring ChromeAction::_getProfileRootDir()
{
	wstring location;
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Google\\Chrome\\User Data";
	return location;
}

const wchar_t* ChromeAction::GetVersion()
{
	if (m_version.length() == 0)
	{
		_readVersion();
	}
	return m_version.c_str();
}

void ChromeAction::CheckPrerequirements(Action * action)
{	
	bool acceptLanguagesOk, localeOk, spellCheckLanguageOk;
	wstring langcode, firstlang;
	
	if (_isInstalled())
	{
		_readVersion();
		
		acceptLanguagesOk = m_chromeProfile->IsAcceptLanguagesOk();
		localeOk = m_chromeProfile->IsUiLocaleOk();
		spellCheckLanguageOk = m_chromeProfile->IsSpellCheckerLanguageOk();

		if(acceptLanguagesOk && localeOk && spellCheckLanguageOk)
		{
			SetStatus(AlreadyApplied);
		}
	} else {
		_setStatusNotInstalled();
	}
}