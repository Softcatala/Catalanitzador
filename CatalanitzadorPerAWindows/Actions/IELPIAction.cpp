/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
#include <stdio.h>

#include "IELPIAction.h"
#include "OSVersion.h"
#include "Url.h"
#include "Winver.h"
#include "FileVersionInfo.h"

IELPIAction::IELPIAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
{
	m_registry = registry;	
	m_OSVersion = OSVersion;
	m_runner = runner;

	m_filename[0] = NULL;
	m_szTempDir[0] = NULL;
	m_version = IEUnread;
}

IELPIAction::~IELPIAction()
{
	if (m_filename[0] != NULL  && GetFileAttributes(m_filename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_filename);
	}

	if (m_szTempDir[0] != NULL)
	{
		RemoveDirectory(m_szTempDir);
	}
}

wchar_t* IELPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_IELPIACTION_NAME, szName);
}

wchar_t* IELPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_IELPIACTION_DESCRIPTION, szDescription);
}

LPCWSTR IELPIAction::GetLicenseID()
{
	switch (_getIEVersion())
	{
		case IE7:
			return MAKEINTRESOURCE(IDR_LICENSE_IE7);
		case IE8:
			return MAKEINTRESOURCE(IDR_LICENSE_IE8);
		case IE9:
			return MAKEINTRESOURCE(IDR_LICENSE_IE9);
		default:
			break;
	}	
	return NULL;
}

IELPIAction::IEVersion IELPIAction::_getIEVersion()
{
	if (m_version == IEUnread)
	{
		m_version = _readIEVersion();
	}
	return m_version;
}

IELPIAction::IEVersion IELPIAction::_readIEVersion()
{
	IEVersion version;	
	wchar_t szVersion[255] = L"";
	wchar_t szMajorVersion[255];
	unsigned int cnt;
	
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer", false))
	{		
		m_registry->GetString(L"Version", szVersion, sizeof(szVersion));
	}
	m_registry->Close();
	
	// read the first part of the number
	for (cnt = 0; cnt < wcslen(szVersion) && (szVersion[cnt] >= L'0' && szVersion[cnt] <= L'9'); cnt++);
	
	if (cnt == 0)
	{
		version = IEUnknown;
	}
	else
	{
		wcsncpy_s(szMajorVersion, szVersion, cnt);

		switch (_wtoi(szMajorVersion))
		{
			case 6:
				version = IE6;
				break;
			case 7:
				version = IE7;
				break;
			case 8:
				version = IE8;
				break;
			case 9:
				version = IE9;
				break;
			default:
				version = IEUnknown;
				break;
		}
	}

	g_log.Log(L"IELPIAction::_readIEVersion returns IE '%u'", (wchar_t *) version);
	return version;
}

DownloadID IELPIAction::_getDownloadID()
{
	switch (_getIEVersion())
	{
		case IE7:
			return DI_IELPI_IE7;
		case IE8:
			return _getDownloadIDIE8();
		case IE9:
			return _getDownloadIDIE9();
		default:
			break;
	}

	return DI_UNKNOWN;
}

DownloadID IELPIAction::_getDownloadIDIE8()
{
	switch (m_OSVersion->GetVersion())
	{
		case WindowsXP:
			return DI_IELPI_IE8_XP;
		case WindowsVista:
			return DI_IELPI_IE8_VISTA;
		default:
			break;
	}
	return DI_UNKNOWN;
}

DownloadID IELPIAction::_getDownloadIDIE9()
{
	switch (m_OSVersion->GetVersion())
	{
		case WindowsVista:
			return DI_IELPI_IE9_VISTA;
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return DI_IELPI_IE9_7_64BITS;
			}
			else
			{
				return DI_IELPI_IE9_7;
			}
		default:
			break;
	}
	return DI_UNKNOWN;
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
	
	Url url(m_actionDownload.GetFileName(_getDownloadID()));
	wcscpy_s(m_filename, m_szTempDir);
	wcscat_s(m_filename, L"\\");
	wcscat_s(m_filename, url.GetFileName());	
	return _getFile(_getDownloadID(), m_filename, progress, data);
}

void IELPIAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";	

	switch (_getIEVersion())
	{
	case IE7:
		{
			wcscpy_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart");
			break;
		}
	case IE8:
		if (m_OSVersion->GetVersion() == WindowsXP)
		{
			wcscpy_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart /update-no");
		}
		else
		{
			GetSystemDirectory(szParams, MAX_PATH);
			wcscat_s(szParams, L"\\wusa.exe ");
			wcscat_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart");
		}
		break;
	case IE9:		
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

		if (_wasInstalled()) 
		{
			SetStatus(Successful);
		}
		else 
		{
			SetStatus(FinishedWithError);
		}

		g_log.Log(L"IELPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool IELPIAction::_wasInstalled()
{
	// Could not find a way to tell if it went well
	if (_getIEVersion() == IE8 && m_OSVersion->GetVersion() == WindowsXP)
	{
		return true;
	}

	return _isLangPackInstalled();
}

IELPIAction::Prerequirements IELPIAction::_checkPrerequirementsDependand(Action * action)
{
	bool WindowsLPISelected;
	WindowsLPISelected = action->GetStatus() == Selected || action->GetStatus() == AlreadyApplied;

	switch (m_OSVersion->GetVersion())
	{
		case WindowsXP: // Includes IE 6
			switch (_getIEVersion())
			{
				case IE6:
					return AppliedInWinLPI;
				case IE7:
				case IE8:
					if (WindowsLPISelected == false)
					{
						return NeedsWinLPI;
					}
					break;
				default:
					break;
			}
			break;
		case WindowsVista: // Includes IE 7
			switch (_getIEVersion())
			{
				case IE7:
					return AppliedInWinLPI;
				case IE8:
				case IE9:
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
				case IE8:
					return AppliedInWinLPI;
				case IE9:				
					if (WindowsLPISelected == false)
					{
						return NeedsWinLPI;
					}
					break;
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
	if (_getIEVersion() == IEUnknown)
		return UnknownIEVersion;
		
	if (m_OSVersion->IsWindows64Bits() && _getIEVersion() != IE9 && _getIEVersion() != IE8)
		return UnknownIEVersion;

	return PrerequirementsOk;
}

void IELPIAction::CheckPrerequirements(Action * action)
{
	szCannotBeApplied[0] = NULL;

	Prerequirements pre;

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
			default:
				break;
		}
	}

	g_log.Log(L"IELPIAction::CheckPrerequirements: %s", wcslen(szCannotBeApplied) > 0 ? szCannotBeApplied: L"Ok");

	if (wcslen(szCannotBeApplied) > 0)
	{
		SetStatus(CannotBeApplied);
	}
	else
	{
		if (_getDownloadID() != DI_UNKNOWN)
		{
			if (_isLangPackInstalled())
			{
				SetStatus(AlreadyApplied);
			}
		}
		else
		{
			SetStatus(CannotBeApplied);
		}
	}
}