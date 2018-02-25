/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "IELPIAction.h"
#include "FileVersionInfo.h"
#include "ConfigurationInstance.h"
#include "LogExtractor.h"

IELPIAction::IELPIAction(IOSVersion* OSVersion, IRunner* runner, IFileVersionInfo* fileVersionInfo, DownloadManager* downloadManager) :
Action(downloadManager), m_explorerVersion(fileVersionInfo), m_multipleDownloads(downloadManager)
{
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_filename[0] = NULL;
	m_szTempDir[0] = NULL;
	m_filenameSpellChecker[0] = NULL;
}

IELPIAction::~IELPIAction()
{
	if (m_filename[0] != NULL  && GetFileAttributes(m_filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename);
	}
	
	if (m_filenameSpellChecker[0] != NULL  && GetFileAttributes(m_filenameSpellChecker) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filenameSpellChecker);
	}

	if (m_szTempDir[0] != NULL)
	{
		RemoveDirectory(m_szTempDir);
	}
}


LPCWSTR IELPIAction::GetLicenseID()
{
	switch (_getIEVersion())
	{
		case InternetExplorerVersion::IE7:
			return MAKEINTRESOURCE(IDR_LICENSE_IE7);
		case InternetExplorerVersion::IE8:
			return MAKEINTRESOURCE(IDR_LICENSE_IE8);
		case InternetExplorerVersion::IE9:
		case InternetExplorerVersion::IE10:
		case InternetExplorerVersion::IE11:
			return MAKEINTRESOURCE(IDR_LICENSE_IE9);
		default:
			break;
	}	
	return NULL;
}

wchar_t* IELPIAction::_getDownloadID()
{
	switch (_getIEVersion())
	{
		case InternetExplorerVersion::IE7:
			return L"IE7";
		case InternetExplorerVersion::IE8:
			return _getDownloadIDIE8();
		case InternetExplorerVersion::IE9:
			return _getDownloadIDIE9();
		case InternetExplorerVersion::IE10:
			return _getDownloadIDIE10();
		case InternetExplorerVersion::IE11:
			return _getDownloadIDIE11();
		default:
			break;
	}

	return NULL;
}

InternetExplorerVersion::IEVersion IELPIAction::_getIEVersion()
{
	return m_explorerVersion.GetVersion();
}

const wchar_t* IELPIAction::GetVersion()
{
	if (m_version.size() == 0)
		m_version = m_explorerVersion.GetVersionString();

	return m_version.c_str();
}

wchar_t* IELPIAction::_getDownloadIDIE8()
{
	switch (m_OSVersion->GetVersion())
	{
		case WindowsVista:
			return L"IE8_VISTA";
		default:
			break;
	}
	return NULL;
}

wchar_t* IELPIAction::_getDownloadIDIE9()
{
	switch (m_OSVersion->GetVersion())
	{
		case WindowsVista:
			return L"IE9_VISTA";
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return L"IE9_7_64";
			}
			else
			{
				return L"IE9_7";
			}
		default:
			break;
	}
	return NULL;
}

wchar_t* IELPIAction::_getDownloadIDIE10()
{
	switch (m_OSVersion->GetVersion())
	{		
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return L"IE10_7_64";
			}
			else
			{
				return L"IE10_7";
			}
		default:
			break;
	}
	return NULL;
}

wchar_t* IELPIAction::_getDownloadIDIE11()
{
	switch (m_OSVersion->GetVersion())
	{		
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return L"IE11_7_64";
			}
			else
			{
				return L"IE11_7";
			}
		default:
			break;
	}
	return NULL;
}

bool IELPIAction::_is64BitsPackage()
{
	switch (m_OSVersion->GetVersion())
	{
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return true;
			}
		default:
			return false;
	}
}

#define CATALAN_LANGCODE 0x403

bool IELPIAction::_isLangPackInstalled()
{
	bool installed;
	wchar_t szFile[MAX_PATH];

	GetSystemDirectory(szFile, MAX_PATH);
	wcscat_s(szFile, L"\\ca-es\\ieframe.dll.mui");

	FileVersionInfo fileVersion;
	fileVersion.SetFilename(szFile);
	installed = _getIEVersion() == fileVersion.GetMajorVersion() && fileVersion.GetLanguageCode() == CATALAN_LANGCODE;

	g_log.Log(L"IELPIAction::_isLangPackInstalled returns %u", (wchar_t*) installed);
	return installed;
}

bool IELPIAction::IsNeed()
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
	g_log.Log(L"IELPIAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());
	return bNeed;
}

// We need to copy the file into a subdirectory with in the temp file since the IE installer 
// will try to do the same in the %temp% dir and fail since the file already exists	
bool IELPIAction::_createTempDirectory()
{
	wchar_t szTempDir[MAX_PATH];

	GetTempPath(MAX_PATH, szTempDir);

	// Unique temporary file (needs to create it)
	GetTempFileName(szTempDir, L"CAT", 0, m_szTempDir);
	DeleteFile(m_szTempDir);

	if (CreateDirectory(m_szTempDir, NULL) == FALSE)
	{
		g_log.Log(L"IELPIAction::_createTempDirectory. Cannot create temp directory '%s'", m_szTempDir);
		return false;
	}
	return true;
}

bool IELPIAction::Download(ProgressStatus progress, void *data)
{
	if (_createTempDirectory() == false)
		return false;

	wstring sha1;
	Sha1Sum sha1sum;
	ConfigurationFileActionDownload downloadVersion;

	m_multipleDownloads.EmptyList();
	if (_canInstallSpellChecker())
	{
		downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(), L"IE11_7_SpellChecker");
		GetTempPath(MAX_PATH, m_filenameSpellChecker);
		wcscat_s(m_filenameSpellChecker, downloadVersion.GetFilename().c_str());
		m_multipleDownloads.AddDownload(downloadVersion, m_filenameSpellChecker);
	}
	
	downloadVersion = ConfigurationInstance::Get().GetRemote().GetDownloadForActionID(GetID(),  _getDownloadID());
	wcscpy_s(m_filename, m_szTempDir);
	wcscat_s(m_filename, L"\\");
	wcscat_s(m_filename, downloadVersion.GetFilename().c_str());
	m_multipleDownloads.AddDownload(downloadVersion, m_filename);
	return m_multipleDownloads.Download(progress, data);
}

bool IELPIAction::_canInstallSpellChecker()
{
	return _getIEVersion() == InternetExplorerVersion::IE11 && m_OSVersion->GetVersion() == Windows7;
}

void IELPIAction::_installSpellChecker()
{
	if (!_canInstallSpellChecker())
		return;

	wchar_t szParams[MAX_PATH] = L"";
	bool is64BitsPackage = false;

	GetSystemDirectory(szParams, MAX_PATH);
	wcscat_s(szParams, L"\\wusa.exe ");
	wcscat_s(szParams, m_filenameSpellChecker);
	wcscat_s(szParams, L" /quiet /norestart");

	g_log.Log(L"IELPIAction::_installSpellChecker '%s', 64 bits %u", szParams, (wchar_t *)is64BitsPackage);
	m_runner->Execute(NULL, szParams, is64BitsPackage);
	m_runner->WaitUntilFinished();
}

void IELPIAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";	

	switch (_getIEVersion())
	{
	case InternetExplorerVersion::IE7:
		{
			wcscpy_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart");
			break;
		}
	case InternetExplorerVersion::IE8:
		GetSystemDirectory(szParams, MAX_PATH);
		wcscat_s(szParams, L"\\wusa.exe ");
		wcscat_s(szParams, m_filename);
		wcscat_s(szParams, L" /quiet /norestart");
		
		break;
	case InternetExplorerVersion::IE9:
	case InternetExplorerVersion::IE10:
	case InternetExplorerVersion::IE11:
			GetSystemDirectory(szParams, MAX_PATH);
			wcscat_s(szParams, L"\\wusa.exe ");
			wcscat_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart");
			break;
	default:
		break;
	}

	SetStatus(InProgress);
	g_log.Log(L"IELPIAction::Execute '%s', 64 bits %u", szParams,  (wchar_t *)_is64BitsPackage());
	m_runner->Execute(NULL, szParams, _is64BitsPackage());
}

ActionStatus IELPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangPackInstalled()) 
		{
			_installSpellChecker();
			SetStatus(Successful);
		}
		else 
		{
			SetStatus(FinishedWithError);
			_dumpWindowsUpdateErrors();
		}

		g_log.Log(L"IELPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}


IELPIAction::Prerequirements IELPIAction::_checkPrerequirementsDependand(Action * action)
{
	bool WindowsLPISelected;
	WindowsLPISelected = action->GetStatus() == Selected || action->GetStatus() == AlreadyApplied;

	switch (m_OSVersion->GetVersion())
	{
		case WindowsVista: // Includes IE 7
			switch (_getIEVersion())
			{
				case InternetExplorerVersion::IE7:
					return AppliedInWinLPI;
				case InternetExplorerVersion::IE8:
				case InternetExplorerVersion::IE9:
					if (WindowsLPISelected == false)
					{
						return NeedsWinLPI;
					}
					break;
				default:
					break;
			}
			break;
		case Windows7: // Includes IE 8
			switch (_getIEVersion())
			{
				case InternetExplorerVersion::IE8:
				{				
					return AppliedInWinLPI;
				}
				case InternetExplorerVersion::IE9:
				case InternetExplorerVersion::IE10:
				case InternetExplorerVersion::IE11:
					if (WindowsLPISelected == false)
					{
						return NeedsWinLPI;
					}					
					break;
				default:
					break;
			}
			break;
		case Windows8: // Includes IE 10
			switch (_getIEVersion())
			{
				case InternetExplorerVersion::IE10:
					return AppliedInWinLPI;
				default:
					break;
			}
			break;
		case Windows81: // Includes IE 11
		case Windows10: // Still included
			switch (_getIEVersion())
			{
				case InternetExplorerVersion::IE11:
					return AppliedInWinLPI;
				default:
					break;
			}
			break;
		default: //	Windows2008, Windows2008R2 and others			
			return NoLangPackAvailable;
	}

	return PrerequirementsOk;
}

IELPIAction::Prerequirements IELPIAction::_checkPrerequirements()
{
	if (_getIEVersion() == InternetExplorerVersion::IEUnknown)
		return UnknownIEVersion;
		
	if (m_OSVersion->IsWindows64Bits() &&
			_getIEVersion() != InternetExplorerVersion::IE11 &&
			_getIEVersion() != InternetExplorerVersion::IE10 &&
			_getIEVersion() != InternetExplorerVersion::IE9 &&
			_getIEVersion() != InternetExplorerVersion::IE8)
		return UnknownIEVersion;

	return PrerequirementsOk;
}

void IELPIAction::CheckPrerequirements(Action * action)
{
	Prerequirements pre;
	ActionStatus status;
	bool versionSupported;

	versionSupported = _getDownloadID() != NULL;

	if (versionSupported)
	{
		if (_isLangPackInstalled())
		{
			SetStatus(AlreadyApplied);
			return;
		}
	}

	if (GetStatus() == CannotBeApplied)
		status = Selected;
	else
		status = GetStatus();

	szCannotBeApplied[0] = NULL;
	pre = _checkPrerequirements();

	if (pre != PrerequirementsOk)
	{
		switch (pre)
		{
			case UnknownIEVersion:
				_getStringFromResourceIDName(IDS_IELPIACTION_UNKNOWNIE, szCannotBeApplied);
				break;
			default:
				break;
		}
	}
	else if (action != NULL)
	{
		switch (_checkPrerequirementsDependand(action))
		{
			case AppliedInWinLPI:
				_getStringFromResourceIDName(IDS_IELPIACTION_APPLIEDINWINLPI, szCannotBeApplied);
				break;
			case NeedsWinLPI:
				_getStringFromResourceIDName(IDS_IELPIACTION_IENEEDWINLPI, szCannotBeApplied);
				break;
			case NoLangPackAvailable:
				_getStringFromResourceIDName(IDS_IELPIACTION_NOPACKAGE, szCannotBeApplied);
				break;
			case UnknownIEVersion:
				_getStringFromResourceIDName(IDS_IELPIACTION_UNKNOWNIE, szCannotBeApplied);
				break;
			default:
				break;
		}
	}

	g_log.Log(L"IELPIAction::CheckPrerequirements: %s", wcslen(szCannotBeApplied) > 0 ? szCannotBeApplied: L"Ok");

	if (wcslen(szCannotBeApplied) > 0)
	{
		status = CannotBeApplied;
	}
	else
	{
		if (versionSupported == false)
		{
			_getStringFromResourceIDName(IDS_IELPIACTION_NOPACKAGE, szCannotBeApplied);
			status = CannotBeApplied;
		}
	}
	SetStatus(status);
}


#define LOG_FILENAME L"WindowsUpdate.log"
#define KEYWORD_TOSEARCH L"Error"
#define LINES_TODUMP 7

void IELPIAction::_dumpWindowsUpdateErrors()
{
	if (_getIEVersion() != InternetExplorerVersion::IE9)
		return;

	wchar_t szFile[MAX_PATH];

	GetWindowsDirectory(szFile, MAX_PATH);
	wcscat_s(szFile, L"\\");
	wcscat_s(szFile, LOG_FILENAME);

	LogExtractor logExtractor(szFile, LINES_TODUMP);
	logExtractor.SetFileIsUnicode(false);
	logExtractor.SetExtractLastOccurrence(true);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD_TOSEARCH);
	logExtractor.DumpLines();
}