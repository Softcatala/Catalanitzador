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
#include "LangToolLibreOfficeAction.h"

LangToolLibreOfficeAction::LangToolLibreOfficeAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : 
Action(downloadManager), m_libreOffice(registry), m_apacheOpenOffice(registry)
{
	m_registry = registry;
	m_runner = runner;
	m_szFilename[0] = NULL;
}

LangToolLibreOfficeAction::~LangToolLibreOfficeAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

wchar_t* LangToolLibreOfficeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_NAME, szName);
}

wchar_t* LangToolLibreOfficeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_DESCRIPTION, szDescription);
}

const wchar_t* LangToolLibreOfficeAction::GetVersion()
{
	if (m_version.size() == 0)
	{
		if (m_libreOffice.IsInstalled())
		{
			m_version = L"LOO " + m_libreOffice.GetVersion();
		}
		else if (m_apacheOpenOffice.IsInstalled())
		{
			m_version = L"AOO " + m_apacheOpenOffice.GetVersion();
		}
	}
	return m_version.c_str();
}

bool LangToolLibreOfficeAction::IsNeed()
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
	g_log.Log(L"LangToolLibreOfficeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

void LangToolLibreOfficeAction::Execute()
{
	SetStatus(InProgress);
	m_installingOffice->InstallExtension(m_runner, m_szFilename);	
}

bool LangToolLibreOfficeAction::Download(ProgressStatus progress, void *data)
{
	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(L"1.0.0"));

	if (downloadVersion.IsEmpty())
	{
		g_log.Log(L"LangToolLibreOfficeAction::Download. ConfigurationFileActionDownload empty");
		return true;
	}

	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());
	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}


#define JAVA_REGKEY L"SOFTWARE\\JavaSoft\\Java Runtime Environment"

bool LangToolLibreOfficeAction::_readJavaVersion(wstring& version)
{
	version.erase();
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, JAVA_REGKEY, false))
	{
		wchar_t szVersion[1024];
	
		if (m_registry->GetString(L"CurrentVersion", szVersion, sizeof(szVersion)))
		{
			version = szVersion;
		}
		m_registry->Close();
	}	

	g_log.Log(L"LangToolLibreOfficeAction::_readVersion version %s", (wchar_t*) version.c_str());
	return version.empty() == false;
}

ActionStatus LangToolLibreOfficeAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (m_installingOffice->IsInstalled())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
		}
		
		g_log.Log(L"LangToolLibreOfficeAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

ApplicationVersion g_javaMinVersion (L"1.7");
#define EXTENSION_NAME L"org.languagetool.openoffice.Main"

void LangToolLibreOfficeAction::CheckPrerequirements(Action * action) 
{
	wstring javaStrVersion, libreOfficeVersion;
	bool bLibreInstalled, bApacheInstalled;

	bLibreInstalled = m_libreOffice.IsInstalled();
	bApacheInstalled = m_apacheOpenOffice.IsInstalled();

	g_log.Log(L"LangToolLibreOfficeAction::CheckPrerequirements: Libreoffice %u, Apacheoffice %u'",
		(wchar_t*) bLibreInstalled, (wchar_t*) bApacheInstalled);
	
	if (bLibreInstalled == false && bApacheInstalled == false)
	{
		wcscpy_s(szCannotBeApplied, L"El LibreOffice o l'Apache OpenOffice no es troben instal·lats");
		SetStatus(CannotBeApplied);
		return;
	}	
	
	if (bLibreInstalled)
	{
		if (m_libreOffice.IsExtensionInstalled(EXTENSION_NAME))
		{
			SetStatus(AlreadyApplied);
			return;
		}
		else
		{
			m_installingOffice = &m_libreOffice;
		}
	}
	else if (bApacheInstalled)
	{
		if (m_apacheOpenOffice.IsExtensionInstalled(EXTENSION_NAME))
		{
			SetStatus(AlreadyApplied);
			return;
		}
		else
		{
			m_installingOffice = &m_apacheOpenOffice;
		}
	}

	if (_readJavaVersion(javaStrVersion) == false)
	{
		wcscpy_s(szCannotBeApplied, L"L'entorn Java no està instal·lat");
		SetStatus(CannotBeApplied);
		return;
	}

	ApplicationVersion javaVersion(javaStrVersion);

	if (javaVersion < g_javaMinVersion)
	{
		wcscpy_s(szCannotBeApplied, L"Versió no existent");
		SetStatus(CannotBeApplied);
		return;
	}
}