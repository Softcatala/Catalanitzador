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
#include "CatalanitzadorUpdateAction.h"
#include "ConfigurationInstance.h"

CatalanitzadorUpdateAction::CatalanitzadorUpdateAction(IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_runner = runner;
	wcscpy_s(szName, L"Catalanitzador");
	szDescription[0] = NULL;
	m_noRunningCheck = false;
}

wchar_t* CatalanitzadorUpdateAction::GetName()
{
	return szName;
}

wchar_t* CatalanitzadorUpdateAction::GetDescription()
{
	return szDescription;
}

bool CatalanitzadorUpdateAction::IsNeed()
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
	g_log.Log(L"CatalanitzadorUpdateAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

bool CatalanitzadorUpdateAction::Download(ProgressStatus progress, void *data)
{
	wchar_t szFilename[MAX_PATH];
	ConfigurationFileActionDownload downloadVersion;
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(GetVersion()));
	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, downloadVersion.GetFilename().c_str());
	m_filename = szFilename;
	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, szFilename, progress, data);
}

#define PARAMETER_NOCHECK L"/NoRunningCheck:"

void CatalanitzadorUpdateAction::Execute()
{
	wstring application(m_filename);

	if (m_noRunningCheck == false)
	{
		application += L" ";
		application += PARAMETER_NOCHECK;
		application += GetVersion();
	}
	else
	{
		if (m_commandline.empty() == false)
		{
			application += L" ";
			application += m_commandline;
		}
	}

	g_log.Log(L"CatalanitzadorUpdateAction::Execute '%s'", (wchar_t *)application.c_str());
	m_runner->Execute(NULL, (wchar_t *)application.c_str(), false);
}

const wchar_t* CatalanitzadorUpdateAction::GetVersion()
{
	if (m_version.empty() == false)
	{
		return m_version.c_str();
	}

	return ConfigurationInstance::Get().GetVersion().GetString().c_str();
}

bool CatalanitzadorUpdateAction::_isRunningInstanceUpToDate()
{
	vector <ConfigurationFileActionDownloads> m_fileActionsDownloads = ConfigurationInstance::Get().GetRemote().GetFileActionsDownloads();
	for (unsigned int i = 0; i < m_fileActionsDownloads.size(); i++)
	{
		if (m_fileActionsDownloads.at(i).GetActionID() == CatalanitzadorUpdateActionID)
		{
			if (m_fileActionsDownloads.at(i).GetFileActionDownloadCollection().size() > 0)
			{
				ConfigurationFileActionDownload fileActionDownload;
				fileActionDownload = m_fileActionsDownloads.at(i).GetFileActionDownloadCollection()[0];
				return ConfigurationInstance::Get().GetVersion() >= fileActionDownload.GetMaxVersion();			
			}
		}
	}
	return true;
}

void CatalanitzadorUpdateAction::CheckPrerequirements(Action * action)
{
	if (GetStatus() != Successful && _isRunningInstanceUpToDate())
	{
		SetStatus(AlreadyApplied);
		return;
	}
}
