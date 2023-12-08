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
#include <regex>
#include "AdobeReaderAction.h"
#include "Runner.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"
#include "Registry.h"

AdobeReaderAction::AdobeReaderAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;	
	m_runner = runner;
	m_szFilename[0]=NULL;
	m_executionStep = ExecutionStepNone;

	GetTempPath(MAX_PATH, m_szTempPath);
	_initProcessNames();
	m_is64Bits = false;
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

#define UNINSTALL_REGKEY L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"


bool AdobeReaderAction::_isdisplayNameFound(wstring path, wstring name, bool is64bits)
{
	bool found = false;
	wstring key = path;

	IRegistry* registry;
	registry = (IRegistry*)new Registry();
	bool result;

	if (is64bits)
		result = registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);
	else
		result = registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);

	if (result)
	{
		wchar_t szDisplayName[2048];
		wstring value;

		if (registry->GetString(L"DisplayName", szDisplayName, sizeof(szDisplayName)))
		{
			registry->Close();

			value = szDisplayName;

			g_log.Log(L"AdobeReaderAction::_isdisplayNameFound: name: '%s'", szDisplayName);

			if (value.compare(0, name.length(), name) == 0)
				found = true;
		}
	}
	delete registry;
	return found;
}

void AdobeReaderAction::_getInstallRegKey(wstring& key)
{
	key = L"";

	_getInstallRegKey_internal(key, true);
	if (key.size() > 0)
	{
		m_is64Bits = true;
		return;
	}
	
	_getInstallRegKey_internal(key, false);
	m_is64Bits = false;
}

void AdobeReaderAction::_getInstallRegKey_internal(wstring& _key, bool is64bits)
{
	bool bKeys = true;
	DWORD dwIndex = 0;
	bool result;

	if (is64bits)
		result = m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, UNINSTALL_REGKEY, false);
	else
		result = m_registry->OpenKey(HKEY_LOCAL_MACHINE, UNINSTALL_REGKEY, false);

	if (result)
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			wstring fullkey = UNINSTALL_REGKEY;
			fullkey += L"\\";
			fullkey += key;
			
			if (_isdisplayNameFound(fullkey, L"Adobe Acrobat", is64bits))
			{ 
				_key = fullkey;
				break;
			}
		}
		m_registry->Close();
	}
}

void AdobeReaderAction::_readInstalledLang(wstring key)
{
	bool result;

	if (m_is64Bits)
		result = m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);
	else
		result = m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);

	if (result)
	{
		DWORD dwLang;

		if (m_registry->GetDWORD(L"Language", &dwLang))
		{
			m_lang = dwLang;
		}
		m_registry->Close();
	}
}

void AdobeReaderAction::_readVersionInstalled()
{
	wstring key;

	_getInstallRegKey(key);

	if (key.size() > 0)
	{
		_readInstalledLang(key);
		_readUninstallGUID(key);
	}
	//g_log.Log(L"AdobeReaderAction::_readVersionInstalled: version: '%s', lang: '%s', versions installed: '%u'", 
	//	(wchar_t *) m_version.c_str(), (wchar_t *) m_lang.c_str(), (wchar_t *) versions.size());
}

void AdobeReaderAction::_readUninstallGUID(wstring key)
{	
	bool result;

	if (m_is64Bits)
		result = m_registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);
	else
		result = m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*)key.c_str(), false);

	if (result)
	{
		wstring uninstall;
		wchar_t szUninstall[2048] = L"";
		wstring value;

		// Since type is REG_EXPAND_SZ instead of REG_SZ this returns error even if reads the text
		m_registry->GetString(L"UninstallString", szUninstall, sizeof(szUninstall));
		if (wcslen(szUninstall) > 0)
		{
			uninstall = szUninstall;
			std::wregex pattern(L"\\{([^\\}]+)\\}");
			std::wsmatch matches;
			if (std::regex_search(uninstall, matches, pattern))
			{
				m_GUID = matches[1];
			}
		
		}
		m_registry->Close();
	}
	g_log.Log(L"AdobeReaderAction::_readUninstallGUID: '%s'", (wchar_t *) m_GUID.c_str());
}

void AdobeReaderAction::FinishExecution(ExecutionProcess process)
{	
	Runner runner;
	vector <DWORD> processIDs = _getProcessIDs(process.GetName(), false);

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

	wcscpy_s(szParams, L" /x {");
	wcscat_s(szParams, m_GUID.c_str());
	wcscat_s(szParams, L"} /quiet /passive");
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
	return m_lang == 0x403;
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

		//_readInstalledLang(m_version);
		_readVersionInstalled();

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

	if (m_GUID.size() > 0)
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