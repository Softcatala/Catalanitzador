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
#include "FirefoxMozillaServer.h"

FirefoxAction::FirefoxAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;
	m_runner = runner;
	m_acceptLanguages = NULL;
	m_mozillaServer = NULL;
	m_szFilename[0] = NULL;

	_addExecutionProcess(ExecutionProcess(L"firefox.exe", L"", true));
}

FirefoxAction::~FirefoxAction()
{
	if (m_acceptLanguages)
		delete m_acceptLanguages;

	if (m_mozillaServer)
		delete m_mozillaServer;
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

FirefoxAcceptLanguages * FirefoxAction::_getAcceptLanguages()
{
	if (m_acceptLanguages == NULL)
	{
		_readVersionAndLocale();
		m_acceptLanguages = new FirefoxAcceptLanguages(_getProfileRootDir(), m_locale);
	}
	return m_acceptLanguages;
}

FirefoxMozillaServer * FirefoxAction::_getMozillaServer()
{
	if (m_mozillaServer == NULL)
	{
		_readVersionAndLocale();
		m_mozillaServer = new FirefoxMozillaServer(m_downloadManager, GetVersion());
	}
	return m_mozillaServer;
}

bool FirefoxAction::IsNeed()
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
	g_log.Log(L"FirefoxAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());	
	return bNeed;
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
	return _isLocaleInstalled() == false;
}

bool FirefoxAction::Download(ProgressStatus progress, void *data)
{
	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;
	
	downloadVersion = _getMozillaServer()->GetConfigurationFileActionDownload();

	if (downloadVersion.IsEmpty())
	{
		g_log.Log(L"FirefoxAction::Download. ConfigurationFileActionDownload empty");
		return true;
	}

	sha1 = _getMozillaServer()->GetSha1FileSignature(downloadVersion);
	sha1sum.SetFromString(sha1);
	
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());
	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, sha1sum, progress, data);
}

void FirefoxAction::Execute()
{	
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	SetStatus(InProgress);
	
	wcscpy_s(szApp, m_szFilename);
	wcscat_s(szApp, L" /s");	
	g_log.Log(L"FirefoxAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(NULL, szApp);
}

ActionStatus FirefoxAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		m_version.clear(); // Invalidate cache
		if (_isAcceptLanguageOk() == false)
		{
			_getAcceptLanguages()->Execute();
		}

		if (_isAcceptLanguageOk() && _isLocaleInstalled())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
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

bool FirefoxAction::_readVersionAndLocale()
{
	if (m_version.length() > 0)
	{
		return true;
	}

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Mozilla\\Mozilla Firefox", false) == false)
	{
		g_log.Log(L"FirefoxAction::_readVersionAndLocale. Cannot open registry key");
		return false;
	}
	
	wchar_t szVersion[1024];
	
	if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
	{
		_extractLocaleAndVersion(szVersion);
		
		g_log.Log(L"FirefoxAction::_readVersionAndLocale. Firefox version %s, version %s, locale %s", 
			(wchar_t*) szVersion, (wchar_t*) m_version.c_str(), (wchar_t*) m_locale.c_str());
	}
	m_registry->Close();
	return m_locale.empty() != true;
}

bool FirefoxAction::_isLocaleInstalled()
{
	bool isInstalled;

	_readVersionAndLocale();
	isInstalled = m_locale == L"ca";

	g_log.Log(L"FirefoxAction::_isLocaleInstalled: %u", (wchar_t*) isInstalled);
	return isInstalled;
}

bool FirefoxAction::_isAcceptLanguageOk()
{
	bool isOk = false;

	if (_getAcceptLanguages()->ReadLanguageCode())
	{
		if (_getAcceptLanguages()->IsNeed() == false)
		{			
			isOk = true;
		}
	}

	g_log.Log(L"FirefoxAction::_isAcceptLanguageOk: %u", (wchar_t*) isOk);
	return isOk;	
}

void FirefoxAction::CheckPrerequirements(Action * action)
{
	_readVersionAndLocale();

	if (_getAcceptLanguages()->ReadLanguageCode() == false)
	{
		_setStatusNotInstalled();
		return;
	}

	if (_isAcceptLanguageOk() && _isLocaleInstalled())
	{
		SetStatus(AlreadyApplied);
		return;
	}
}