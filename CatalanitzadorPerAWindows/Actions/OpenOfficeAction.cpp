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
#include "OpenOfficeAction.h"
#include "Url.h"

OpenOfficeAction::OpenOfficeAction(IRegistry* registry, IRunner* runner)
{
	m_registry = registry;	
	m_runner = runner;
	m_szFilename[0]=NULL;
	m_szTempPathCAB[0] = NULL;

	GetTempPath(MAX_PATH, m_szTempPath);
}

OpenOfficeAction::~OpenOfficeAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}

	_removeCabTempFiles();
}

wchar_t* OpenOfficeAction::GetName()
{
	return _getStringFromResourceIDName(IDS_OPENOFFICEACTION_NAME, szName);
}

wchar_t* OpenOfficeAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_OPENOFFICEACTION_DESCRIPTION, szDescription);
}

#define PROGRAM_REGKEY L"SOFTWARE\\OpenOffice.org\\OpenOffice.org"

// TODO: You can several versions installed, just read first one for now
void OpenOfficeAction::_readVersionInstalled()
{
	bool bKeys = true;
	DWORD dwIndex = 0;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, PROGRAM_REGKEY, false))
	{
		while (bKeys)
		{
			wstring key;

			bKeys = m_registry->RegEnumKey(dwIndex, key);
			dwIndex++;

			if (bKeys)
			{
				m_version = key;
				break;
			}
		}
		m_registry->Close();
	}
	g_log.Log(L"OpenOfficeAction::_readVersionInstalled '%s'", (wchar_t *) m_version.c_str());
}

bool OpenOfficeAction::IsNeed()
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
	g_log.Log(L"OpenOfficeAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

bool OpenOfficeAction::Download(ProgressStatus progress, void *data)
{
	GetTempPath(MAX_PATH, m_szFilename);
	Url url(m_actionDownload.GetFileName(DI_OPENOFFICE_33));
	wcscat_s(m_szFilename, url.GetFileName());	
	return _getFile(DI_OPENOFFICE_33, m_szFilename, progress, data);	
}

void OpenOfficeAction::Execute()
{	
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	// Unique temporary file (needs to create it)
	GetTempFileName(m_szTempPath, L"CAT", 0, m_szTempPathCAB);
	DeleteFile(m_szTempPathCAB);

	if (CreateDirectory(m_szTempPathCAB, NULL) == FALSE)
	{
		g_log.Log(L"OpenOfficeAction::Execute. Cannot create temp directory '%s'", m_szTempPathCAB);
		return;
	}

	_extractCabFile(m_szFilename, m_szTempPathCAB);

	wcscpy_s(szApp, m_szTempPathCAB);
	wcscat_s(szApp, L"\\setup.exe /q");
	status = InProgress;	
	g_log.Log(L"OpenOfficeAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(NULL, szApp);
}

bool OpenOfficeAction::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");

	swprintf_s (szParams, L" %s %s -f:*", file, path);
	g_log.Log(L"OpenOfficeAction::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute(szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

// This deletes the contents of the extracted CAB file for MS Office 2003
void OpenOfficeAction::_removeCabTempFiles()
{
	if (m_szTempPathCAB[0] == NULL)
		return;
	
	wchar_t szFile[MAX_PATH];
	wchar_t* files[] = {L"openofficeorg1.cab", L"openofficeorg33.msi", L"setup.exe", L"setup.ini", NULL};

	for(int i = 0; files[i] != NULL; i++)
	{
		wcscpy_s(szFile, m_szTempPathCAB);
		wcscat_s(szFile, L"\\");
		wcscat_s(szFile, files[i]);

		if (DeleteFile(szFile) == FALSE)
		{
			g_log.Log(L"OpenOfficeAction::_removeCabTempFiles. Cannot delete '%s'", (wchar_t *) szFile);
		}		
	}

	RemoveDirectory(m_szTempPathCAB);
}

bool OpenOfficeAction::_isLangPackInstalled()
{
	wstring key;
	bool bRslt = false;

	key = PROGRAM_REGKEY;
	key += L"\\";
	key += m_version;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, (wchar_t*) key.c_str(), false))
	{
		int i = 0;
		wchar_t szFileName[MAX_PATH];

		m_registry->GetString(L"path", szFileName, sizeof(szFileName));
		m_registry->Close();

		for (i = wcslen(szFileName); i > 0 && szFileName[i] != '\\' ; i--);
		
		szFileName[i + 1] = NULL;
		wcscat_s(szFileName, L"resource\\oooca.res");

		bRslt = GetFileAttributes(szFileName) != INVALID_FILE_ATTRIBUTES;
	}
	g_log.Log(L"OpenOfficeAction::_isLangPackInstalled '%u'", (wchar_t *) bRslt);
	return bRslt;
}

ActionStatus OpenOfficeAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangPackInstalled()) 
		{
			status = Successful;
		}
		else {
			status = FinishedWithError;			
		}
		
		g_log.Log(L"OpenOfficeAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

void OpenOfficeAction::CheckPrerequirements(Action * action)
{
	_readVersionInstalled();

	if (m_version.size() > 0)
	{
		if (_isLangPackInstalled() == true)
		{
			SetStatus(AlreadyApplied);
			return;
		}

		if (m_version != L"3.3")
		{			
			_getStringFromResourceIDName(IDS_OPENOFFICEACTION_NOTSUPPORTEDVERSION, szCannotBeApplied);
			g_log.Log(L"OpenOfficeAction::CheckPrerequirements. Version not supported");
			SetStatus(CannotBeApplied);
			return;
		}
	}
	else
	{
		SetStatus(NotInstalled);
		return;
	}
}
