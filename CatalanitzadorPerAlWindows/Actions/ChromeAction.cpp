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
	m_registry = registry;

	_addExecutionProcess(ExecutionProcess(L"chrome.exe", L"", true));
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
	bool uiStatus = _chromeProfile.IsUiLocaleOk();
	bool acceptLanguageStatus;

	if(!uiStatus) {
		uiStatus = _chromeProfile.WriteUILocale();
	}

	SetStatus(InProgress);
	acceptLanguageStatus = _chromeProfile.UpdateAcceptLanguages();

	if(uiStatus && acceptLanguageStatus) {
		SetStatus(Successful);
	} else {
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

	if (m_registry->OpenKey(HKEY_CURRENT_USER, CHROME_REGISTRY_PATH, false))
	{
		wchar_t szLocation[MAX_PATH];
		
		if (m_registry->GetString(L"InstallLocation", szLocation, sizeof(szLocation)))
		{
			path = szLocation;
			
			_chromeProfile.SetPath(path);
			g_log.Log(L"ChromeAction::_readInstallLocation. InstallLocation %s", szLocation);		
		}		
		m_registry->Close();
	} 
	else 
	{
		g_log.Log(L"ChromeAction::_readInstallLocation - Chrome is not installed");
	}
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
	bool acceptLanguagesOk, localeOk;
	wstring langcode, firstlang;
	
	if (_isInstalled())
	{

		_readVersion();
		
		acceptLanguagesOk = _chromeProfile.IsAcceptLanguagesOk();
		localeOk = _chromeProfile.IsUiLocaleOk();

		if(acceptLanguagesOk && localeOk)
		{
			SetStatus(AlreadyApplied);
		}
	} else {
		_setStatusNotInstalled();
	}
}

bool ChromeAction::_readLanguageCode(std::wstring &langcode)
{
	if(_isInstalled())
	{
		return _chromeProfile.ReadLanguageCode(langcode);
	}
	return false;
}