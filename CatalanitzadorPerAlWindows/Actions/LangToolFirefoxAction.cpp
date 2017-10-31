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
#include "ConfigurationInstance.h"
#include "LangToolFirefoxAction.h"

const wchar_t* APPLICATION_ID_GUID = L"languagetool-webextension@languagetool.org";

LangToolFirefoxAction::LangToolFirefoxAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : 
Action(downloadManager), m_firefox(registry, &m_OSversion)
{
	m_registry = registry;
	m_runner = runner;
	m_szFilename[0] = NULL;
	m_firefoxAddOn = NULL;
}

LangToolFirefoxAction::~LangToolFirefoxAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}

	if (m_firefoxAddOn)
	{
		delete m_firefoxAddOn;
	}
}

wchar_t* LangToolFirefoxAction::GetName()
{
	return _getStringFromResourceIDName(IDS_LANGUAGETOOL_FIREFOX_NAME, szName);
}

wchar_t* LangToolFirefoxAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_LANGUAGETOOL_FIREFOX_DESCRIPTION, szName);
}

FirefoxAddOn* LangToolFirefoxAction::_getFirefoxAddon()
{
	if (!m_firefoxAddOn)
		m_firefoxAddOn = new FirefoxAddOn(m_firefox.GetUserDataDirectory(), m_firefox.GetProfileRootDir(), m_firefox.GetInstallPath());

	return m_firefoxAddOn;
}

const wchar_t* LangToolFirefoxAction::GetVersion()
{
	m_version = m_firefox.GetVersion();
	return m_version.c_str();
}

bool LangToolFirefoxAction::IsNeed()
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
	g_log.Log(L"LangToolFirefoxAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

void LangToolFirefoxAction::Execute()
{
	SetStatus(InProgress);

	wstring location = _getFirefoxAddon()->InstallAddOn(APPLICATION_ID_GUID, m_szFilename);

	if (_getFirefoxAddon()->IsAddOnInstalled(APPLICATION_ID_GUID))
	{
		SetStatus(Successful);
	}
	else
	{
		SetStatus(FinishedWithError);
	}

	g_log.Log(L"LangToolFirefoxAction::Execute is '%s'", GetStatus() == Successful ? L"Successful" : L"FinishedWithError");
}

bool LangToolFirefoxAction::Download(ProgressStatus progress, void *data)
{	
	wstring filename;
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(GetVersion()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}

ActionStatus LangToolFirefoxAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;
		
		g_log.Log(L"LangToolFirefoxAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

void LangToolFirefoxAction::CheckPrerequirements(Action * action) 
{
	m_version = m_firefox.GetVersion();

	if (m_version.size() > 0 && _getFirefoxAddon()->CanInstallAddOns())
	{
		if (_getFirefoxAddon()->IsAddOnInstalled(APPLICATION_ID_GUID))
		{
			SetStatus(AlreadyApplied);
			return;
		}
		
		// Is version supported
		ConfigurationFileActionDownload downloadVersion;
		
		downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(m_version));
		if (downloadVersion.IsUsable() == false)
		{
			_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"LangToolFirefoxAction::CheckPrerequirements. Version not supported");
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