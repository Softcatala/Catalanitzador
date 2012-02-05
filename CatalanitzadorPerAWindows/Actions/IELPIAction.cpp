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
#include <stdio.h>

#include "IELPIAction.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Registry.h"
#include "Url.h"
#include "Winver.h"

IELPIAction::IELPIAction()
{	
	m_filename[0] = NULL;
	m_szTempDir[0] = NULL;
	m_version = _getVersion();	
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

IEVersion IELPIAction::_getVersion()
{
	IEVersion version;
	Registry registry;
	wchar_t szVersion[128] = L"";
	wchar_t szMajorVersion[128];
	unsigned int cnt;
	
	if (registry.OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer", false))
	{		
		registry.GetString(L"Version", szVersion, sizeof(szVersion));
	}
	registry.Close();
	
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

	g_log.Log(L"IELPIAction::_getVersion returns IE '%u'", (wchar_t *) version);
	return version;
}

DownloadID IELPIAction::_getDownloadID()
{
	switch (m_version)
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
	switch (m_osVersion.GetVersion())
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
	switch (m_osVersion.GetVersion())
	{
		case WindowsVista:
			return DI_IELPI_IE9_VISTA;
		case Windows7:
			if (m_osVersion.IsWindows64Bits())
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
	switch (m_osVersion.GetVersion())
	{
		case Windows7:
			if (m_osVersion.IsWindows64Bits())
			{
				return true;
			}
		default:
			return false;
	}
}

struct LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} *lpTranslate;

#define CATALAN_LANGCODE 0x403

bool IELPIAction::_isLangPackInstalled()
{
	bool installed = false;
	wchar_t szFile[MAX_PATH];
	DWORD dwLen, dwUnUsed;
	LPTSTR lpVI;

	GetSystemDirectory(szFile, MAX_PATH);
	wcscat_s(szFile, L"\\ca-es\\ieframe.dll.mui");
	
	dwLen = GetFileVersionInfoSize(szFile, &dwUnUsed);

	if (dwLen == 0)
	{
		g_log.Log(L"IELPIAction::_isLangPackInstalled returns false. Cannot getfileinfo for %s", szFile);
		return false;
	}		

	lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);
	if (lpVI != NULL)
	{		
		GetFileVersionInfo(szFile, NULL, dwLen, lpVI);
		unsigned int uLen = sizeof(LANGANDCODEPAGE);

		VerQueryValue(lpVI,
			L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate,  &uLen);
		
		VS_FIXEDFILEINFO *lpFfi;
		VerQueryValue(lpVI , L"\\" , (LPVOID *)&lpFfi , &uLen);
		WORD majorVersion = HIWORD (lpFfi->dwFileVersionMS);

		installed = (m_version == majorVersion && lpTranslate->wLanguage == CATALAN_LANGCODE);	

		g_log.Log(L"IELPIAction::_isLangPackInstalled %s has version %u and language code %x", 
			szFile, (wchar_t*) majorVersion, (wchar_t*) lpTranslate->wLanguage);
		
		GlobalFree((HGLOBAL)lpVI);
	}

	g_log.Log(L"IELPIAction::_isLangPackInstalled returns %u", (wchar_t*) installed);
	return installed;
}

bool IELPIAction::IsNeed()
{
	if (status == CannotBeApplied)
		return false;
	
	bool bNeed = false;

	if (_getDownloadID() != DI_UNKNOWN)
	{
		if (_isLangPackInstalled() == false)
		{		
			bNeed = true;
		}
		else
		{
			status = AlreadyApplied;
		}		
	}
	else
	{
		status = CannotBeApplied;
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDVERSION, szCannotBeApplied);
		g_log.Log(L"IELPIAction::IsNeed. Unsupported Windows version");
	}

	g_log.Log(L"IELPIAction::IsNeed returns %u", (wchar_t *) bNeed);
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
	
	Url url(m_downloadAction.GetFileName(_getDownloadID()));
	wcscpy_s(m_filename, m_szTempDir);
	wcscat_s(m_filename, L"\\");
	wcscat_s(m_filename, url.GetFileName());	
	return _getFile(_getDownloadID(), m_filename, progress, data);
}

void IELPIAction::Execute()
{
	wchar_t szParams[MAX_PATH] = L"";	

	switch (m_version)
	{
	case IE7:
		{
			wcscpy_s(szParams, m_filename);
			wcscat_s(szParams, L" /quiet /norestart");
			break;
		}
	case IE8:
		if (m_osVersion.GetVersion() == WindowsXP)
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

	status = InProgress;
	g_log.Log(L"IELPIAction::Execute '%s', 64 bits %u", szParams,  (wchar_t *)_is64BitsPackage());
	runner.Execute (NULL, szParams, _is64BitsPackage());
}

ActionStatus IELPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (runner.IsRunning())
			return InProgress;

		if (_wasInstalled()) {
			status = Successful;
		}
		else {
			status = FinishedWithError;			
		}
		
		g_log.Log(L"IELPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool IELPIAction::_wasInstalled()
{
	// Could not find a way to tell if it went well
	if (m_version == IE8 && m_osVersion.GetVersion() == WindowsXP)
	{
		return true;
	}

	return _isLangPackInstalled();
}

void IELPIAction::CheckPrerequirements(Action * action)
{
	szCannotBeApplied[0] = NULL;

	if (m_version == IEUnknown || (m_osVersion.IsWindows64Bits() && m_version != IE9))
	{
		_getStringFromResourceIDName(IDS_IELPIACTION_UNKNOWNIE, szCannotBeApplied);
	}
	else if (action != NULL)
	{
		bool WindowsLPISelected;
		WindowsLPISelected = action->GetStatus() == Selected || action->GetStatus() == AlreadyApplied;
	
		switch (m_osVersion.GetVersion())
		{
			case WindowsXP: // Includes IE 6
				switch (m_version)
				{
					case IE6:
						_getStringFromResourceIDName(IDS_IELPIACTION_APPLIEDINWINLPI, szCannotBeApplied);
						break;
					case IE7:
					case IE8:				
						if (WindowsLPISelected == false)
						{
							_getStringFromResourceIDName(IDS_IELPIACTION_IENEEDWINLPI, szCannotBeApplied);
						}
						break;
					default:
						break;
				}
				break;
			case WindowsVista: // Includes IE 7
				switch (m_version)
				{
					case IE7:				
						_getStringFromResourceIDName(IDS_IELPIACTION_APPLIEDINWINLPI, szCannotBeApplied);
						break;
					case IE8:
					case IE9:				
						if (WindowsLPISelected == false)
						{
							_getStringFromResourceIDName(IDS_IELPIACTION_IENEEDWINLPI, szCannotBeApplied);
						}
						break;
					default:
						break;
				}
				break;
			case Windows7: // Includes IE 8
				switch (m_version)
				{
					case IE8:				
						_getStringFromResourceIDName(IDS_IELPIACTION_APPLIEDINWINLPI, szCannotBeApplied);
						break;
					case IE9:				
						if (WindowsLPISelected == false)
						{
							_getStringFromResourceIDName(IDS_IELPIACTION_IENEEDWINLPI, szCannotBeApplied);
						}
						break;
					default:
						break;
				}
				break;
			default: //	Windows2008, Windows2008R2 and others
				_getStringFromResourceIDName(IDS_IELPIACTION_NOPACKAGE, szCannotBeApplied);
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
		if (_getDownloadID() != DI_UNKNOWN)
		{
			if (_isLangPackInstalled() == false)
			{		
				status = Selected;
			}
			else
			{
				status = AlreadyApplied;
			}		
		}
		else
		{
			status = CannotBeApplied;
		}
	}
}