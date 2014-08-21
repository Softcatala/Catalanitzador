/* 
 * Copyright (C) 2013-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

LangToolLibreOfficeAction::LangToolLibreOfficeAction(IRegistry* registry, IRunner* runner, IOpenOffice* libreOffice, IOpenOffice* apacheOpenOffice, DownloadManager* downloadManager) :
Action(downloadManager), m_multipleDownloads(downloadManager)
{
	m_registry = registry;
	m_runner = runner;
	m_libreOffice = libreOffice;
	m_apacheOpenOffice = apacheOpenOffice;
	m_szFilename[0] = NULL;
	m_szFilenameJava[0] = NULL;
	m_executionStep = ExecutionStepNone;
}

LangToolLibreOfficeAction::~LangToolLibreOfficeAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}

	if (m_szFilenameJava[0] != NULL  && GetFileAttributes(m_szFilenameJava) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilenameJava);
	}
}

wchar_t* LangToolLibreOfficeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_NAME, szName);
}

wchar_t* LangToolLibreOfficeAction::GetDescription()
{
	_getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_DESCRIPTION, szDescription);
	
	if (m_shouldInstallJava)
	{
		wchar_t szJava[MAX_LOADSTRING];

		_getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_REQUIERESJAVA, szJava);
		wcscat_s(szDescription, L"\r\n\r\n");
		wcscat_s(szDescription, szJava);
	}
	
	return szDescription;
}

const wchar_t* LangToolLibreOfficeAction::GetVersion()
{
	if (m_version.size() == 0)
	{
		if (m_libreOffice->IsInstalled())
		{
			m_version = L"LOO " + m_libreOffice->GetVersion();
		}
		else if (m_apacheOpenOffice->IsInstalled())
		{
			m_version = L"AOO " + m_apacheOpenOffice->GetVersion();
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

void LangToolLibreOfficeAction::_installJava()
{
	wstring app, params;	

	app = m_szFilenameJava;
	params = L" /s";	

	g_log.Log(L"LangToolLibreOfficeAction::_installJava. '%s' with params '%s'", (wchar_t*) app.c_str(), (wchar_t*) params.c_str());
	m_runner->Execute((wchar_t*) app.c_str(), (wchar_t*) params.c_str());	
}

void LangToolLibreOfficeAction::Execute()
{
	SetStatus(InProgress);

	if (m_shouldInstallJava)
	{
		_installJava();
	}
	else
	{
		m_installingOffice->InstallExtension(m_runner, m_szFilename);	
	}

	SetStatus(InProgress);
	m_executionStep = ExecutionStep1;
}

bool LangToolLibreOfficeAction::Download(ProgressStatus progress, void *data)
{
	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;

	if (m_shouldInstallJava)
	{
		downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), L"Java");
		GetTempPath(MAX_PATH, m_szFilenameJava);
		wcscat_s(m_szFilenameJava, downloadVersion.GetFilename().c_str());
		m_multipleDownloads.AddDownload(downloadVersion, m_szFilenameJava);		
	}
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), L"LanguageTool");
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());
	m_multipleDownloads.AddDownload(downloadVersion, m_szFilename);
	return m_multipleDownloads.Download(progress, data);
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

	return version.empty() == false;
}

ActionStatus LangToolLibreOfficeAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		switch (m_executionStep)
		{
			case ExecutionStepNone:
				break;
			case ExecutionStep1:
			{
				if (m_shouldInstallJava)
				{
					m_installingOffice->InstallExtension(m_runner, m_szFilename);	
					m_executionStep = ExecutionStep2;
					return InProgress;
				}
				break;
			}				
			case ExecutionStep2:
				break;
			default:
				assert(false);
				break;
		}

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

bool LangToolLibreOfficeAction::_isOpenOfficeInstalled(bool& bLibreInstalled, bool& bApacheInstalled)
{
	bLibreInstalled = m_libreOffice->IsInstalled();
	bApacheInstalled = m_apacheOpenOffice->IsInstalled();

	g_log.Log(L"LangToolLibreOfficeAction::_isOpenOfficeInstalled: Libreoffice %u, ApacheOpenOffice %u",
		(wchar_t*) bLibreInstalled, (wchar_t*) bApacheInstalled);
	
	return bLibreInstalled || bApacheInstalled;
}

bool LangToolLibreOfficeAction::_isExtensionInstalled(bool bLibreInstalled, bool bApacheInstalled)
{
	if (bLibreInstalled)
	{
		if (m_libreOffice->IsExtensionInstalled(EXTENSION_NAME))
		{
			return true;
		}		
	}
	else if (bApacheInstalled)
	{
		if (m_apacheOpenOffice->IsExtensionInstalled(EXTENSION_NAME))
		{		
			return true;
		}		
	}
	return false;
}

bool LangToolLibreOfficeAction::_shouldInstallJava()
{
	wstring javaStrVersion;
	bool bShouldInstallJava;

	_readJavaVersion(javaStrVersion);
	bShouldInstallJava = ApplicationVersion(javaStrVersion) < g_javaMinVersion;
	
	g_log.Log(L"LangToolLibreOfficeAction::_shouldInstallJava. Java version %s, should install %u",
		(wchar_t*) javaStrVersion.c_str(), (wchar_t*) bShouldInstallJava);

	return bShouldInstallJava;
}

bool LangToolLibreOfficeAction::_doesJavaNeedsConfiguration()
{
	bool bShouldConfigureJava;

	wstring javaConfigured = m_installingOffice->GetJavaConfiguredVersion();
	ApplicationVersion appJavaConfigured(javaConfigured);
	bShouldConfigureJava = appJavaConfigured < g_javaMinVersion;

	g_log.Log(L"LangToolLibreOfficeAction::_doesJavaNeedsConfiguration: %u", (wchar_t*) bShouldConfigureJava);
	return bShouldConfigureJava;
}

void LangToolLibreOfficeAction::CheckPrerequirements(Action * action) 
{
	bool bLibreInstalled, bApacheInstalled;

	if (_isOpenOfficeInstalled(bLibreInstalled, bApacheInstalled) == false)
	{
		_getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_CANNOTINSTALL, szCannotBeApplied);
		SetStatus(NotInstalled);
		return;
	}			

	if (_isExtensionInstalled(bLibreInstalled, bApacheInstalled))
	{
		SetStatus(AlreadyApplied);
		return;
	}

	if (bLibreInstalled)
	{
		m_installingOffice = m_libreOffice;	
	}
	else if (bApacheInstalled)
	{
		m_installingOffice = m_apacheOpenOffice;		
	}

	m_shouldInstallJava = _shouldInstallJava();
	m_shouldConfigureJava = _doesJavaNeedsConfiguration();
}