/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FirefoxLangPackAction.h"
#include "FirefoxMozillaServer.h"
#include "FirefoxChannel.h"

FirefoxLangPackAction::FirefoxLangPackAction(IRunner* runner, wstring installPath, wstring locale, wstring version, DownloadManager* downloadManager) : Action(downloadManager)
{	
	m_runner = runner;
	m_locale = locale;
	m_version = version;
	m_installPath = installPath;
	m_mozillaServer = NULL;
	m_szFilename[0] = NULL;	
}

FirefoxLangPackAction::~FirefoxLangPackAction()
{
	if (m_mozillaServer)
		delete m_mozillaServer;

	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

FirefoxMozillaServer * FirefoxLangPackAction::_getMozillaServer()
{
	if (m_mozillaServer == NULL)
	{		
		m_mozillaServer = new FirefoxMozillaServer(m_downloadManager, GetVersion());
	}
	return m_mozillaServer;
}

bool FirefoxLangPackAction::IsNeed()
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
	g_log.Log(L"FirefoxLangPackAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

bool FirefoxLangPackAction::IsDownloadNeed()
{
	return _isLocaleInstalled() == false;
}

bool FirefoxLangPackAction::Download(ProgressStatus progress, void *data)
{
	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;
	
	downloadVersion = _getMozillaServer()->GetConfigurationFileActionDownload();

	if (downloadVersion.IsEmpty())
	{
		g_log.Log(L"FirefoxLangPackAction::Download. ConfigurationFileActionDownload empty");
		return true;
	}

	sha1 = _getMozillaServer()->GetSha1FileSignature(downloadVersion);
	sha1sum.SetFromString(sha1);
	
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());
	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, sha1sum, progress, data);
}

void FirefoxLangPackAction::Execute()
{	
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	SetStatus(InProgress);
	
	wcscpy_s(szApp, m_szFilename);
	wcscat_s(szApp, L" /s");	
	g_log.Log(L"FirefoxLangPackAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(NULL, szApp);
}

ActionStatus FirefoxLangPackAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		// Used to signal that InProgress is completed
		SetStatus(FinishedWithError);
	}
	return status;
}

void FirefoxLangPackAction::SetLocaleAndUpdateStatus(wstring locale) 
{
	m_locale = locale;
	
	if (_isLocaleInstalled())
	{
		status = Successful;
	}
	else
	{
		status =  FinishedWithError;
	}
	g_log.Log(L"FirefoxLangPackAction::SetLocaleAndUpdateStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
}

bool FirefoxLangPackAction::_isSupportedChannel()
{
	bool supported;

	FirefoxChannel channel(m_installPath);
	supported = channel.GetChannel() == L"release";
	return supported;
}

bool FirefoxLangPackAction::_isLocaleInstalled()
{
	bool isInstalled;

	isInstalled = m_locale == L"ca";	
	g_log.Log(L"FirefoxLangPackAction::_isLocaleInstalled: %u", (wchar_t*) isInstalled);
	return isInstalled;
}

void FirefoxLangPackAction::CheckPrerequirements(Action * action)
{
	if (_isLocaleInstalled())
	{
		SetStatus(AlreadyApplied);
		return;
	}

	if (_isSupportedChannel() == false)
	{
		g_log.Log(L"FirefoxLangPackAction::CheckPrerequirements. Unsupported channel");
		SetStatus(CannotBeApplied);
		return;
	}

	if (_getMozillaServer()->GetConfigurationFileActionDownload().IsEmpty())
	{
		g_log.Log(L"FirefoxLangPackAction::CheckPrerequirements. Version with no download");
		SetStatus(CannotBeApplied);
		return;
	}
}