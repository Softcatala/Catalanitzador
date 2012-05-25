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
#include "Url.h"
#include "Runner.h"

AdobeReaderAction::AdobeReaderAction(IRegistry* registry, IRunner* runner)
{
	m_registry = registry;	
	m_runner = runner;
	m_szFilename[0]=NULL;
	m_executionStep = ExecutionStepNone;

	GetTempPath(MAX_PATH, m_szTempPath);
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

DownloadID AdobeReaderAction::_getDownloadID()
{
	switch (_getMajorVersion())
	{
		case 9:
			return DI_ADOBEREADER_95;
		case 10:
			return DI_ADOBEREADER_1010;
		default:
			assert(false);
			return DI_ADOBEREADER_1010;
	}
}

bool AdobeReaderAction::Download(ProgressStatus progress, void *data)
{
	GetTempPath(MAX_PATH, m_szFilename);

	Url url(m_actionDownload.GetFileName(_getDownloadID()));
	wcscat_s(m_szFilename, url.GetFileName());	
	return _getFile(_getDownloadID(), m_szFilename, progress, data);	
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
				if (iswdigit(key[0]))
				{
					versions.push_back(key);
				}
			}
		}
		m_registry->Close();
	}
}

void AdobeReaderAction::_readInstalledLang()
{
	wstring key(ACROBAT_REGKEY);

	// TODO: swprintf
	key+=L"\\";
	key+=m_version;
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

	AdobeReaderAction::_enumVersions(versions);	

	if (versions.size() > 0)
	{
		m_version = versions[0];
		_readInstalledLang();
		_readUninstallGUID();
	}
	g_log.Log(L"AdobeReaderAction::_readVersionInstalled: version: '%s', lang: '%s', versions installed: '%u'", 
		(wchar_t *) m_version.c_str(), (wchar_t *) m_lang.c_str(), (wchar_t *) versions.size());
}

int AdobeReaderAction::_getMajorVersion()
{
	size_t pos;

	pos = m_version.find_first_of(L".", 0);

	if (pos == string::npos)
	{
		return -1;
	}

	return _wtoi(m_version.substr(0, pos).c_str());
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

DWORD AdobeReaderAction::_getProcessID()
{
	Runner runner;
	return runner.GetProcessID(wstring(L"AcroRd32.exe"));
}

bool AdobeReaderAction::IsExecuting()
{
	return _getProcessID() != 0;
}

void AdobeReaderAction::FinishExecution()
{
	Runner runner;
	runner.RequestQuitToProcessID(_getProcessID());
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
	wchar_t szParams[MAX_PATH] = L"";
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

		_readInstalledLang();

		if (_isLangPackInstalled())
		{
			SetStatus(Successful);
		}
		else
		{
			SetStatus(FinishedWithError);
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

		int major = _getMajorVersion();
		if (major != 9 && major != 10)
		{
			_getStringFromResourceIDName(IDS_ADOBEREADERACTION_NOTSUPPORTEDVERSION, szCannotBeApplied);
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
