/* 
 * Copyright (C) 2011-2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "MSOfficeLPIAction.h"
#include "Runner.h"
#include "Url.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

#define CATALAN_LCID L"1027" // 0x403
#define VALENCIAN_LCID L"2051" // 0x803

MSOfficeLPIAction::MSOfficeLPIAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager) :
	Action(downloadManager), m_multipleDownloads(downloadManager), m_MSOffice(registry, runner)
{
	m_registry = registry;
	m_runner = runner;
	m_MSVersion = MSOfficeUnKnown;
	m_szFullFilename[0] = NULL;
	m_szTempPath2003[0] = NULL;
	m_szFilename[0] = NULL;
	m_executionStep = ExecutionStepNone;
	m_bLangPackInstalled64bits = false;
	GetTempPath(MAX_PATH, m_szTempPath);
	m_OutLookHotmailConnector = NULL;
}

MSOfficeLPIAction::~MSOfficeLPIAction()
{
	if (m_szFullFilename[0] != NULL  && GetFileAttributes(m_szFullFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFullFilename);
	}

	if (m_msiexecLog.empty() == false && GetFileAttributes(m_msiexecLog.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_msiexecLog.c_str());
	}

	if (m_OutLookHotmailConnector)
		delete m_OutLookHotmailConnector;

	_removeOffice2003TempFiles();
}

wchar_t* MSOfficeLPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_NAME, szName);
}

wchar_t* MSOfficeLPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_MSOFFICEACTION_DESCRIPTION, szDescription);
}

LPCWSTR MSOfficeLPIAction::GetLicenseID()
{
	switch (_getVersionInstalled())
	{
		case MSOffice2003:
			return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2003);
		case MSOffice2007:
			return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2007);
		case MSOffice2010:
		case MSOffice2010_64:
			return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2010);
		case MSOffice2013:
		case MSOffice2013_64:
			return MAKEINTRESOURCE(IDR_LICENSE_OFFICE2013);
		default:
			break;
	}
	return NULL;
}

const wchar_t* MSOfficeLPIAction::GetVersion()
{
	switch (_getVersionInstalled())
	{
		case MSOffice2003:
			return L"2003";
		case MSOffice2007:
			return L"2007";
		case MSOffice2010:
			return L"2010";
		case MSOffice2010_64:
			return L"2010_64bits";
		case MSOffice2013:
			return L"2013";
		case MSOffice2013_64:
			return L"2013_64bits";
		default:
			return L"";
	}
}

// This deletes the contents of the extracted CAB file for MS Office 2003
void MSOfficeLPIAction::_removeOffice2003TempFiles()
{
	if (_getVersionInstalled() != MSOffice2003 || m_szTempPath2003[0] == NULL)
		return;
	
	wchar_t szFile[MAX_PATH];
	wchar_t* files[] = {L"DW20.ADM_1027", L"DWINTL20.DLL_0001_1027", L"LIP.MSI",
		L"lip.xml", L"OSE.EXE", L"SETUP.CHM_1027", L"SETUP.EXE", L"SETUP.INI", L"lip1027.cab",
		NULL};

	for(int i = 0; files[i] != NULL; i++)
	{
		wcscpy_s(szFile, m_szTempPath2003);
		wcscat_s(szFile, L"\\");
		wcscat_s(szFile, files[i]);

		if (DeleteFile(szFile) == FALSE)
		{
			g_log.Log(L"MSOfficeLPIAction::_removeOffice2003TempFiles. Cannot delete '%s'", (wchar_t *) szFile);
		}		
	}

	RemoveDirectory(m_szTempPath2003);
}



bool MSOfficeLPIAction::_isLangPackInstalled()
{
	if (m_bLangPackInstalled.IsUndefined())
	{		
		m_MSOffice._readIsLangPackInstalled(_getVersionInstalled(), m_bLangPackInstalled, m_bLangPackInstalled64bits);
	}
	return m_bLangPackInstalled == true;
}

MSOfficeVersion MSOfficeLPIAction::_getVersionInstalled()
{
	if (m_MSVersion == MSOfficeUnKnown)
	{
		m_MSVersion = m_MSOffice._readVersionInstalled();
	}
	return m_MSVersion;
}


wchar_t*  MSOfficeLPIAction::_getDownloadID()
{
	switch (_getVersionInstalled())
	{
		case MSOffice2003:
			return L"2003";
		case MSOffice2007:
			return L"2007";
		case MSOffice2010:
			return L"2010_32";
		case MSOffice2010_64:
			return L"2010_64";
		case MSOffice2013:
			if (GetUseDialectalVariant())
			{
				return L"2013_va_32";
			}
			else
			{
				return L"2013_ca_32";
			}			
		case MSOffice2013_64:
			if (GetUseDialectalVariant())
			{
				return L"2013_va_64";
			}
			else
			{
				return L"2013_ca_64";
			}
		default:
			return NULL;
	}
}

bool MSOfficeLPIAction::IsNeed()
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
	g_log.Log(L"MSOfficeLPIAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}


bool MSOfficeLPIAction::Download(ProgressStatus progress, void *data)
{
	ConfigurationFileActionDownload downloadVersion;

	assert(_getDownloadID() != NULL);
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), wstring(_getDownloadID()));	
	wcscpy_s(m_szFilename, downloadVersion.GetFilename().c_str());
	wcscpy_s(m_szFullFilename, m_szTempPath);
	wcscat_s(m_szFullFilename, m_szFilename);
	m_multipleDownloads.AddDownload(downloadVersion, m_szFullFilename);

	if (_getOutLookHotmailConnector()->IsNeed())
	{
		wstring tempFile;

		_getOutLookHotmailConnector()->GetDownloadConfigurationAndTempFile(downloadVersion, tempFile);
		m_multipleDownloads.AddDownload(downloadVersion, tempFile);
	}

	return m_multipleDownloads.Download(progress, data);
}

bool MSOfficeLPIAction::_extractCabFile(wchar_t * file, wchar_t * path)
{
	Runner runnerCab;
	wchar_t szParams[MAX_PATH];
	wchar_t szApp[MAX_PATH];
	
	GetSystemDirectory(szApp, MAX_PATH);
	wcscat_s(szApp, L"\\expand.exe ");


	swprintf_s (szParams, L" %s %s -f:*", file, path);
	g_log.Log(L"MSOfficeLPIAction::_extractCabFile '%s' with params '%s'", szApp, szParams);
	runnerCab.Execute(szApp, szParams);
	runnerCab.WaitUntilFinished();
	return true;
}

void MSOfficeLPIAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";
	wchar_t szApp[MAX_PATH] = L"";

	switch (_getVersionInstalled())
	{
		case MSOffice2013_64:
		case MSOffice2013:
		case MSOffice2010_64:
		case MSOffice2010:		
		{
			wchar_t logFile[MAX_PATH];

			wcscpy_s(szApp, m_szFullFilename);
			wcscpy_s(szParams, L" /passive /norestart /quiet");

			GetTempPath(MAX_PATH, logFile);
			wcscat_s(logFile, L"msofficelip.log");
			wcscat_s(szParams, L" /log:");
			wcscat_s(szParams, logFile);
			m_msiexecLog = logFile;
			break;
		}
		case MSOffice2007:
		{
			wcscpy_s(szApp, m_szFullFilename);
			wcscpy_s(szParams, L" /quiet");
			break;
		}
		case MSOffice2003:
		{
			wchar_t szMSI[MAX_PATH];

			// Unique temporary file (needs to create it)
			GetTempFileName(m_szTempPath, L"CAT", 0, m_szTempPath2003);
			DeleteFile(m_szTempPath2003);

			if (CreateDirectory(m_szTempPath2003, NULL) == FALSE)
			{
				g_log.Log(L"MSOfficeLPIAction::Execute. Cannot create temp directory '%s'", m_szTempPath2003);
				break;
			}
		
			_extractCabFile(m_szFullFilename, m_szTempPath2003);

			GetSystemDirectory(szApp, MAX_PATH);
			wcscat_s(szApp, L"\\msiexec.exe ");

			wcscpy_s(szMSI, m_szTempPath2003);
			wcscat_s(szMSI, L"\\");
			wcscat_s(szMSI, L"lip.msi");

			wcscpy_s(szParams, L" /i ");
			wcscat_s(szParams, szMSI);
			wcscat_s(szParams, L" /qn");
			break;
		}
	default:
		break;
	}

	SetStatus(InProgress);
	m_executionStep = ExecutionStep1;
	g_log.Log(L"MSOfficeLPIAction::Execute '%s' with params '%s'", szApp, szParams);
	m_runner->Execute(szApp, szParams);
}


ActionStatus MSOfficeLPIAction::GetStatus()
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
				if (_getOutLookHotmailConnector()->Execute(m_runner) == true)
				{
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
		
		if (m_MSOffice._isLangPackForVersionInstalled(_getVersionInstalled(), m_bLangPackInstalled64bits))		
		{
			SetStatus(Successful);
			m_MSOffice._setDefaultLanguage(_getVersionInstalled());
		}
		else
		{
			SetStatus(FinishedWithError);

			if (m_msiexecLog.empty() == false)
			{
				#define LINES_TODUMP 7
				#define KEYWORD_TOSEARCH L"Error"

				LogExtractor logExtractor(m_msiexecLog, LINES_TODUMP);
				logExtractor.SetExtractLastOccurrence(true);
				logExtractor.SetFileIsUnicode(false);
				logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
				logExtractor.DumpLines();
			}
		}		
		g_log.Log(L"MSOfficeLPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

OutLookHotmailConnector* MSOfficeLPIAction::_getOutLookHotmailConnector()
{
	if (m_OutLookHotmailConnector == NULL)
	{
		bool MSOffice2013OrHigher = _getVersionInstalled() == MSOffice2013 || _getVersionInstalled() == MSOffice2013_64;
		m_OutLookHotmailConnector = new OutLookHotmailConnector(MSOffice2013OrHigher, m_registry);
	}

	return m_OutLookHotmailConnector;
}

void MSOfficeLPIAction::CheckPrerequirements(Action * action) 
{
	if (_getVersionInstalled() == NoMSOffice)
	{
		_setStatusNotInstalled();
		return;
	}

	if (_isLangPackInstalled())
	{
		SetStatus(AlreadyApplied);
		return;
	}
}
