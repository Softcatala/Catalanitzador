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

#define LISTENER_WINDOWCLASS    L"SO Listener Class"
#define KILLTRAY_MESSAGE        L"SO KillTray"
#define SOFFICE_PROCESSNAME		L"soffice.bin"

ApplicationVersion g_javaMinVersion (L"1.7");
#define EXTENSION_NAME L"org.languagetool.openoffice.Main"


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
	m_shouldInstallJava = false;
	m_shouldConfigureJava = false;
	m_installingOffice = NULL;

	_addExecutionProcess(ExecutionProcess(SOFFICE_PROCESSNAME, L"", false));
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

ExecutionProcess LangToolLibreOfficeAction::GetExecutingProcess()
{	
	ExecutionProcess process;

	process = ActionExecution::GetExecutingProcess();

	if (process.IsEmpty() == false)
	{
		return process;
	}

	if (FindWindowEx(NULL, NULL, LISTENER_WINDOWCLASS, NULL) == NULL)
	{
		return ExecutionProcess();
	}
	else
	{
		return ExecutionProcess(KILLTRAY_MESSAGE, L"", true);
	}
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
	m_executionStep = ExecutionStepInstallJava;
}

bool LangToolLibreOfficeAction::Download(ProgressStatus progress, void *data)
{
	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;

	m_multipleDownloads.EmptyList();
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

		g_log.Log(L"LangToolLibreOfficeAction::GetStatus: Process returned %x", (wchar_t*) m_runner->GetExitCode());

		switch (m_executionStep)
		{
			case ExecutionStepNone:
				break;
			case ExecutionStepInstallJava:
			{
				if (m_shouldInstallJava)
				{
					_installJava();
				}
				
				m_executionStep = ExecutionStepInstallExtension;
				return InProgress;
			}
			case ExecutionStepInstallExtension:
			{
				bool shouldInstall;

				if (m_shouldInstallJava)
				{
					bool successfullyInstalled = _shouldInstallJava() == false;					
					shouldInstall = successfullyInstalled;
					g_log.Log(L"LangToolLibreOfficeAction::GetStatus. Java installed successfully %u", (wchar_t*) successfullyInstalled);
				}
				else
				{
					shouldInstall = true;
				}				

				if (shouldInstall)
				{
					m_installingOffice->InstallExtension(m_runner, m_szFilename);
					m_executionStep = ExecutionStepRetryInstallExtension;
					return InProgress;
				}
				
				break;
			}

			// LibreOffice sometimes files instaling and extension but then it works if you just re-entry again
			// Has to do with the fact that the first execution creates directories than then the second can use			
			case ExecutionStepRetryInstallExtension:
			{
				if (m_shouldInstallJava && _shouldInstallJava()) // Was not able to install Java
					break;

				if (m_installingOffice->IsExtensionInstalled(EXTENSION_NAME) == false)
				{					
					m_installingOffice->InstallExtension(m_runner, m_szFilename);
					m_executionStep = ExecutionStepFinished;
					return InProgress;					
				}
				break;
			}
			case ExecutionStepFinished:
				break;
			default:
				assert(false);
				break;
		}

		if (m_installingOffice->IsExtensionInstalled(EXTENSION_NAME))
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

bool LangToolLibreOfficeAction::_isOpenOfficeInstalled(bool& bLibreInstalled, bool& bApacheInstalled)
{
	bLibreInstalled = m_libreOffice->IsInstalled();
	bApacheInstalled = m_apacheOpenOffice->IsInstalled();

	g_log.Log(L"LangToolLibreOfficeAction::_isOpenOfficeInstalled: Libreoffice %u, ApacheOpenOffice %u",
		(wchar_t*) bLibreInstalled, (wchar_t*) bApacheInstalled);
	
	return bLibreInstalled || bApacheInstalled;
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

// If there is only one version of Java OpenOffice can figure it iself
// It is not, you need to selected manually 
bool LangToolLibreOfficeAction::_doesJavaNeedsConfiguration()
{
	bool bShouldConfigureJava, bNoJavaInstalled, bShouldInstallJava;
	wstring javaConfigured, javaStrVersion;
	
	_readJavaVersion(javaStrVersion);
	javaConfigured = m_installingOffice->GetJavaConfiguredVersion();
	bShouldInstallJava = ApplicationVersion(javaStrVersion) < g_javaMinVersion;
	bNoJavaInstalled = javaStrVersion.length() == 0;	
	
	// If Java is not installed we are OK if it is not configured in OpenOffice
	if (bNoJavaInstalled)
	{
		bShouldConfigureJava = javaConfigured.length() > 0;
	}
	else
	{
		// You need to configure Java since you will end up with two
		// runtimes installed. If you have previusly selected Java cannot be
		// for sure the right version since it was not installed
		if (bShouldInstallJava)
		{
			bShouldConfigureJava = true;
		}
		else
		{
			if (javaConfigured.length() == 0)
			{
				// If no Java, and no configured version OpenOffice should be able to pick up
				// the right version
				bShouldConfigureJava = false;
			}
			else
			{
				// If Java is installed, then you should have picked up the right version
				bShouldConfigureJava =  ApplicationVersion(javaConfigured) < g_javaMinVersion;
			}
		}
	}
	
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

	if (bLibreInstalled)
	{
		m_installingOffice = m_libreOffice;
	}
	else if (bApacheInstalled)
	{
		m_installingOffice = m_apacheOpenOffice;
	}

	if (m_installingOffice->IsExtensionInstalled(EXTENSION_NAME))
	{
		SetStatus(AlreadyApplied);
		return;
	}
	
	m_shouldInstallJava = _shouldInstallJava();
	m_shouldConfigureJava = _doesJavaNeedsConfiguration();

	// Configuring the right JRE for OpenOffice if really complex. OpenOffice does not do it iself.
	// It includes determining the correct JRE, vendor, version, etc. It also may affect other installed extensions
	// See: http://cgit.freedesktop.org/libreoffice/core/tree/jvmfwk/source/javasettings_template.xml
	if (m_shouldConfigureJava)
	{
		_getStringFromResourceIDName(IDS_LANGUAGETOOL_LO_CANNOTAUTOMATEINSTALL, szCannotBeApplied);
		g_log.Log(L"LangToolLibreOfficeAction::CheckPrerequirements. Cannot configure automatically");		
		SetStatus(CannotBeApplied);
		return;
	}
}