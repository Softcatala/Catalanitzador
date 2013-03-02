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
#include "FirefoxAction.h"

FirefoxAction::FirefoxAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;
	m_runner = runner;
	m_szFilename[0] = NULL;
	m_cachedVersionAndLocale = false;
	m_firefoxLangPackAction = NULL;
	m_firefoxAcceptLanguagesAction = NULL;
	m_doFirefoxLangPackAction = false;
	m_doFirefoxAcceptLanguagesAction = false;

	_addExecutionProcess(ExecutionProcess(L"firefox.exe", L"", true));
}

FirefoxAction::~FirefoxAction()
{
	if (m_firefoxLangPackAction)
		delete m_firefoxLangPackAction;

	if (m_firefoxAcceptLanguagesAction)
		delete m_firefoxAcceptLanguagesAction;
}

wchar_t* FirefoxAction::GetName()
{
	return _getStringFromResourceIDName(IDS_FIREFOXACTION_NAME, szName);
}

wchar_t* FirefoxAction::GetDescription()
{	
	return _getStringFromResourceIDName(IDS_FIREFOXACTION_DESCRIPTION, szDescription);
}

void FirefoxAction::FinishExecution(ExecutionProcess process)
{
	vector <DWORD> processIDs = _getProcessIDs(process.GetName());

	if (processIDs.size() > 0)
	{
		Runner runner;
		runner.RequestCloseToProcessID(processIDs.at(0), true);
	}
}

FirefoxLangPackAction * FirefoxAction::_getLangPackAction()
{
	if (m_firefoxLangPackAction == NULL)
	{
		wstring path;

		_readInstallPath(path);		
		m_firefoxLangPackAction = new FirefoxLangPackAction(m_runner, path, _getLocale(), GetVersion(), m_downloadManager);
	}
	return m_firefoxLangPackAction;
}

FirefoxAcceptLanguagesAction * FirefoxAction::_getAcceptLanguagesAction()
{
	if (m_firefoxAcceptLanguagesAction == NULL)
	{
		m_firefoxAcceptLanguagesAction = new FirefoxAcceptLanguagesAction(_getProfileRootDir(), _getLocale(), GetVersion());
	}
	return m_firefoxAcceptLanguagesAction;
}

bool FirefoxAction::IsNeed()
{
	bool bNeed;

	m_doFirefoxLangPackAction = _getLangPackAction()->IsNeed();
	m_doFirefoxAcceptLanguagesAction = _getAcceptLanguagesAction()->IsNeed();

	bNeed = m_doFirefoxLangPackAction || m_doFirefoxAcceptLanguagesAction;

	g_log.Log(L"FirefoxAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());	
	return bNeed;
}

void FirefoxAction::SetStatus(ActionStatus value)
{
	Action::SetStatus(value);
	if (value == Selected || value == NotSelected)
	{
		_getLangPackAction()->SetStatus(value);
		_getAcceptLanguagesAction()->SetStatus(value);
	}
}

wstring FirefoxAction::_getProfileRootDir()
{
	wstring location;
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Mozilla\\Firefox\\";
	return location;
}

bool FirefoxAction::IsDownloadNeed()
{
	if (m_doFirefoxLangPackAction)
	{
		return _getLangPackAction()->IsDownloadNeed();
	}
	else
	{
		return false;
	}
}

bool FirefoxAction::Download(ProgressStatus progress, void *data)
{
	return _getLangPackAction()->Download(progress, data);
}

void FirefoxAction::Execute()
{	
	if (m_doFirefoxLangPackAction)
	{
		_getLangPackAction()->Execute();
	}
	SetStatus(InProgress);
}

ActionStatus FirefoxAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_doFirefoxLangPackAction && _getLangPackAction()->GetStatus() == InProgress)
			return InProgress;

		// Re-read locale after language pack
		if (m_doFirefoxLangPackAction)
		{
			m_cachedVersionAndLocale = false;
			_getLangPackAction()->SetLocaleAndUpdateStatus(_getLocale());
		}

		if (m_doFirefoxAcceptLanguagesAction)
		{
			_getAcceptLanguagesAction()->Execute();
		}

		if (m_doFirefoxLangPackAction && _getLangPackAction()->GetStatus() != Successful ||
			m_doFirefoxAcceptLanguagesAction && _getAcceptLanguagesAction()->GetStatus() != Successful)			
		{
			SetStatus(FinishedWithError);
		}
		else
		{
			SetStatus(Successful);
		}
		
		g_log.Log(L"FirefoxAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

const wchar_t* FirefoxAction::GetVersion()
{
	_readVersionAndLocale();	
	return m_version.c_str();
}

wstring FirefoxAction::_getLocale()
{	
	_readVersionAndLocale();	
	return m_locale;
}

void FirefoxAction::_extractLocaleAndVersion(wstring version)
{	
	int start, end;

	start = version.find(L" ");
	if (start != wstring::npos)
	{
		m_version = version.substr(0, start);			

		start = version.find(L"(", start);

		if (start != wstring::npos)
		{
			start++;
			end = version.find(L")", start);
			if (end != wstring::npos)
			{
				m_locale = version.substr(start, end-start);
			}
		}
	}
}

#define FIREFOX_REGKEY L"SOFTWARE\\Mozilla\\Mozilla Firefox"

wstring FirefoxAction::_getVersionAndLocaleFromRegistry()
{
	wstring version;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, FIREFOX_REGKEY, false) == false)
	{
		g_log.Log(L"FirefoxAction::_getVersionAndLocaleFromRegistry. Cannot open registry key");
		return version;
	}
	
	wchar_t szVersion[1024];
	
	if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
	{
		g_log.Log(L"FirefoxAction::_getVersionAndLocaleFromRegistry. Firefox version %s", (wchar_t*) szVersion);
		version = szVersion;
	}

	m_registry->Close();
	return version;
}

void FirefoxAction::_readVersionAndLocale()
{
	if (m_cachedVersionAndLocale)
		return;

	wstring version;
	version = _getVersionAndLocaleFromRegistry();
	_extractLocaleAndVersion(version);
	m_cachedVersionAndLocale = true;
}

void FirefoxAction::_readInstallPath(wstring& path)
{
	wstring version;

	version = _getVersionAndLocaleFromRegistry();

	if (version.empty())
		return;

	wstring key(FIREFOX_REGKEY);
	key += L"\\" + version + L"\\Main";

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false) == false)
	{
		g_log.Log(L"FirefoxAction::_readInstallPath. Cannot open registry key");
		return;
	}
	
	wchar_t szPath[MAX_PATH];
	
	if (m_registry->GetString(L"Install Directory", szPath, sizeof(szPath)))
	{
		path = szPath;
	}
	m_registry->Close();
}

void FirefoxAction::Serialize(ostream* stream)
{
	_getLangPackAction()->Serialize(stream);
	_getAcceptLanguagesAction()->Serialize(stream);
}

void FirefoxAction::CheckPrerequirements(Action * action)
{
	if (wcslen(GetVersion()) == 0)
	{
		_setStatusNotInstalled();
		return;
	}

	_getLangPackAction()->CheckPrerequirements(action);
	_getAcceptLanguagesAction()->CheckPrerequirements(action);

	if (_getLangPackAction()->GetStatus() == AlreadyApplied &&
		_getAcceptLanguagesAction()->GetStatus() == AlreadyApplied)
	{
			SetStatus(AlreadyApplied);
			return;
	}

	// We do not have a good way of communication when one subactions cannot be applied
	// but the other can
	if (_getLangPackAction()->GetStatus() == CannotBeApplied &&
		_getAcceptLanguagesAction()->GetStatus() == AlreadyApplied)
	{
			SetStatus(AlreadyApplied);
			return;
	}
}