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
#include "AdobeReaderAction.h"
#include "Runner.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

AdobeReaderAction::AdobeReaderAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;	
	m_runner = runner;
	m_szFilename[0]=NULL;
	m_executionStep = ExecutionStepNone;

	GetTempPath(MAX_PATH, m_szTempPath);
	_initProcessNames();
}

void AdobeReaderAction::_initProcessNames()
{
	// Cannot close Adobe: in some scenarios the window is closed but the process is left open (and the user cannot close it)
	_addExecutionProcess(ExecutionProcess(L"AcroRd32.exe", L"", false));
	_addExecutionProcess(ExecutionProcess(L"iexplore.exe", L"Internet Explorer", true));

	//See: http://answers.microsoft.com/en-us/windows/forum/windows_xp-windows_programs/when-trying-to-install-adobe-acrobat-x-get-error/c8a03501-e160-4dac-b983-ae19f9a973bc
	_addExecutionProcess(ExecutionProcess(L"searchfilterhost.exe", L"Microsoft Windows Search", true));
}

AdobeReaderAction::~AdobeReaderAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

wchar_t* AdobeReaderAction::GetName()
{	
	return _getStringFromResourceIDName(IDS_ADOBEREADERCTION_NAME, szName);
}

wchar_t* AdobeReaderAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_ADOBEREADERCTION_DESCRIPTION, szDescription);	
}

bool AdobeReaderAction::Download(ProgressStatus progress, void *data)
{	
	wstring filename;
	ConfigurationFileActionDownload downloadVersion;

	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(GetVersion()));
	GetTempPath(MAX_PATH, m_szFilename);
	wcscat_s(m_szFilename, downloadVersion.GetFilename().c_str());

	return m_downloadManager->GetFileAndVerifyAssociatedSha1(downloadVersion, m_szFilename, progress, data);
}

#define ACROBAT_REGKEY L"Software\\Adobe\\Acrobat Reader"

void AdobeReaderAction::_enumVersions(vector <wstring>& versions)
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, ACROBAT_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				// Sometimes there are keys like WSZXSGANXFJVAYSXYQGNXKQY. May be broken installers.
				if (iswdigit(key[0]) || key == L"DC")
				{
					versions.push_back(key);
				}
			}
		}
		m_registry->Close();
	}
}

void AdobeReaderAction::_readInstalledLang(wstring version)
{
	wstring key(ACROBAT_REGKEY);

	// TODO: swprintf
	key+=L"\\";
	key+=version;
	key+=L"\\";
	key+=L"Language";

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false))
	{
		wchar_t szLang[2048];

		if (m_registry->GetString(L"UI", szLang, sizeof(szLang)))
		{
			m_lang = szLang;
		}
		m_registry->Close();
	}
}

void AdobeReaderAction::_readVersionInstalled()
{
	vector <wstring> versions;

	_enumVersions(versions);

	if (versions.size() > 0)
	{
		m_version = versions[0];
		_readInstalledLang(m_version);
		_readUninstallGUID();
	}
	g_log.Log(L"AdobeReaderAction::_readVersionInstalled: version: '%s', lang: '%s', versions installed: '%u'", 
		(wchar_t *) m_version.c_str(), (wchar_t *) m_lang.c_str(), (wchar_t *) versions.size());
}

void AdobeReaderAction::_readUninstallGUID()
{
	wstring key(ACROBAT_REGKEY);
	
	key+=L"\\";
	key+=m_version;
	key+=L"\\";
	key+=L"Installer";
	
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false))
	{
		wchar_t szGUID[2048];

		if (m_registry->GetString(L"ENU_GUID", szGUID, sizeof(szGUID)))
		{
			m_GUID = szGUID;
		}
		m_registry->Close();
	}
	g_log.Log(L"AdobeReaderAction::_readUninstallGUID: '%s'", (wchar_t *) m_GUID.c_str());
}

void AdobeReaderAction::FinishExecution(ExecutionProcess process)
{	
	Runner runner;
	vector <DWORD> processIDs = _getProcessIDs(process.GetName());

	for (unsigned int i = 0; i < processIDs.size(); i++)
	{
		runner.TerminateProcessID(processIDs.at(i));
	}
}

bool AdobeReaderAction::IsNeed()
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
	g_log.Log(L"AdobeReaderAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

void AdobeReaderAction::_uninstall()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\msiexec.exe ");

	wcscpy_s(szParams, L" /x");
	wcscat_s(szParams, m_GUID.c_str());
	wcscat_s(szParams, L" /quiet /passive");
	m_executionStep = ExecutionStep1;
	
	g_log.Log(L"AdobeReaderAction::_uninstall '%s' with params '%s'", szApp, szParams);
	m_hideApplicationWindow.Start();
	m_runner->Execute(szApp, szParams);
}

void AdobeReaderAction::_installVersion()
{
	wchar_t szApp[MAX_PATH] = L"";	

	wcscpy_s(szApp, m_szFilename);
	wcscat_s(szApp, L" /rs /sAll");
	m_executionStep = ExecutionStep2;	
	g_log.Log(L"AdobeReaderAction::Execute '%s'", szApp);
	m_runner->Execute(NULL, szApp);
}

void AdobeReaderAction::Execute()
{	
	int ids[] = {2, 8, 0xb, 0xa, NULL};

	m_hideApplicationWindow.AddControlsID(ids);
	SetStatus(InProgress);
	_uninstall();
}

bool AdobeReaderAction::_isLangPackInstalled()
{
	return m_lang.compare(L"CAT") == 0;
}

ActionStatus AdobeReaderAction::GetStatus()
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
				m_hideApplicationWindow.Stop();
				_installVersion();
				return InProgress;
			}
			case ExecutionStep2:
				break;
			default:
				assert(false);
				break;
		}

		_readInstalledLang(m_version);

		if (_isLangPackInstalled())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
			_dumpInstallerErrors();
		}
		g_log.Log(L"AdobeReaderAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

void AdobeReaderAction::CheckPrerequirements(Action * action)
{
	_readVersionInstalled();

	if (m_version.size() > 0)
	{
		if (_isLangPackInstalled() == true)
		{
			SetStatus(AlreadyApplied);
			return;
		}
		
		ConfigurationFileActionDownload downloadVersion;
		
		downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), ApplicationVersion(m_version));
		if (downloadVersion.IsUsable() == false)
		{
			_getStringFromResourceIDName(IDS_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"AdobeReaderAction::CheckPrerequirements. Version not supported");
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

#define LOG_FILENAME L"AdobeSFX.log"
#define KEYWORD_TOSEARCH L"Error"
#define LINES_TODUMP 7

void AdobeReaderAction::_dumpInstallerErrors()
{	
	wchar_t szFile[MAX_PATH];

	GetTempPath(MAX_PATH, szFile);
	wcscat_s(szFile, LOG_FILENAME);

	LogExtractor logExtractor(szFile, LINES_TODUMP);
	logExtractor.SetFileIsUnicode(false);
	logExtractor.SetExtractLastOccurrence(true);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
	logExtractor.DumpLines();
}