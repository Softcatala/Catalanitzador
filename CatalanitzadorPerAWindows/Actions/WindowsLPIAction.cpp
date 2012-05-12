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

#include "WindowsLPIAction.h"
#include "OSVersion.h"
#include "Runner.h"
#include "Url.h"
#include "Resources.h"

WindowsLPIAction::WindowsLPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
{
	m_registry = registry;
	m_win32I18N = win32I18N;
	m_OSVersion = OSVersion;
	m_runner = runner;
	m_szFilename[0] = NULL;
}

WindowsLPIAction::~WindowsLPIAction()
{
	if (m_szFilename[0] != NULL  && GetFileAttributes(m_szFilename) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(m_szFilename);
	}
}

wchar_t* WindowsLPIAction::GetName()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NAME, szName);
}

wchar_t* WindowsLPIAction::GetDescription()
{
	return _getStringFromResourceIDName(IDS_WINDOWSLPIACTION_DESCRIPTION, szDescription);
}

LPCWSTR WindowsLPIAction::GetLicenseID()
{
	OperatingVersion version = m_OSVersion->GetVersion();

	switch (version)
	{
		case WindowsXP:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWSXP);
		case WindowsVista:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWSVISTA);
		case Windows7:
			return MAKEINTRESOURCE(IDR_LICENSE_WINDOWS7);
		default:
			break;
	}
	return NULL;
}

DownloadID WindowsLPIAction::_getDownloadID()
{
	OperatingVersion version = m_OSVersion->GetVersion();

	switch (version)
	{
		case WindowsXP:
		{
			WORD majorVersion = HIWORD(m_OSVersion->GetServicePackVersion());

			// The Windows LPI for SP2 or higher checks if the the Windows is validated and fails if is not
			// If the version of Windows is not validated, we switch back to the old language pack that does not
			// do any validation
			if (majorVersion >= 2 && IsWindowsValidated())
			{
				return DI_WINDOWSLPIACTION_XP_SP2;
			}
			else
			{
				return DI_WINDOWSLPIACTION_XP;
			}
		}

		case WindowsVista:		
			return DI_WINDOWSLPIACTION_VISTA;
		case Windows7:
			if (m_OSVersion->IsWindows64Bits())
			{
				return DI_WINDOWSLPIACTION_7_64BITS;
			}
			else
			{
				return DI_WINDOWSLPIACTION_7;
			}
		default:
			break;
	}
	return DI_UNKNOWN;
}

#define LANGUAGE_CODE L"ca-ES"

bool WindowsLPIAction::IsDownloadNeed()
{
	return _isLangPackInstalled() == false;
}

// The langpack may be installed by not selected
bool WindowsLPIAction::_isDefaultLanguage()
{
	if (m_OSVersion->GetVersion() == WindowsXP)
		return true;

	wchar_t szPreferred[2048] =L"";	
	wchar_t szPreferredPending[2048] =L"";	
	
	// Sets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", false))
	{
		m_registry->GetString(L"PreferredUILanguages", szPreferred, sizeof(szPreferred));
		m_registry->GetString(L"PreferredUILanguagesPending", szPreferredPending, sizeof(szPreferredPending));
		m_registry->Close();
	}
	g_log.Log(L"WindowsLPIAction::_isDefaultLanguage preferred lang '%s', preferred pending lang '%s'", 
		szPreferred, szPreferredPending);

	return (_wcsnicmp(szPreferred, LANGUAGE_CODE, wcslen(LANGUAGE_CODE)) == 0) ||
		(_wcsnicmp(szPreferredPending, LANGUAGE_CODE, wcslen(LANGUAGE_CODE)) == 0);
}

// Checks if the Catalan language pack is already installed
// This code works if the langpack is installed or has just been installed (and the user did not reboot)
bool WindowsLPIAction::_isLangPackInstalled()
{	
	bool bExists;

	OperatingVersion version = m_OSVersion->GetVersion();

	if (version == WindowsXP)
	{
		bExists = false;
		if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\", false))
		{
			wchar_t szValue[1024];		

			// MultiUILanguageId key is left behind
			if (m_registry->GetString(L"MUILanguagePending", szValue, sizeof (szValue)))
			{
				if (wcsstr(szValue, L"0403") != NULL)
					bExists = true;
			}
			m_registry->Close();
		}
	}
	else  //(version == WindowsVista) or 7
	{		
		bExists = m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\UILanguages\\ca-ES", false);
		m_registry->Close();
	}		
	
	g_log.Log (L"WindowsLPIAction::_isLangPackInstalled returns %u", (wchar_t*) bExists);
	return bExists;
}

bool WindowsLPIAction::IsNeed()
{
 	if (status == CannotBeApplied)
		return false;
	
	bool bNeed = false;

	if (_getDownloadID() != DI_UNKNOWN)
	{		
		if (_isLangPackInstalled() == false || _isDefaultLanguage() == false)
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
		g_log.Log(L"WindowsLPIAction::IsNeed. Unsupported Windows version");
	}

	g_log.Log(L"WindowsLPIAction::IsNeed returns %u", (wchar_t *) bNeed);
	return bNeed;	
}

bool WindowsLPIAction::Download(ProgressStatus progress, void *data)
{
	GetTempPath(MAX_PATH, m_szFilename);

	Url url(m_actionDownload.GetFileName(_getDownloadID()));
	wcscat_s(m_szFilename, url.GetFileName());
	return _getFile(_getDownloadID(), m_szFilename, progress, data);
}

void WindowsLPIAction::Execute()
{
	wchar_t szParams[MAX_PATH];
	wchar_t lpkapp[MAX_PATH];

	if (_isLangPackInstalled() == true)
	{
		_setDefaultLanguage();
		status = _isLangPackInstalled() ? Successful : FinishedWithError;
		g_log.Log(L"WindowsLPIAction::Execute. Setting default language only was '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
		return;
	}

	OperatingVersion version = m_OSVersion->GetVersion();

	if (version == WindowsXP)
	{
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\msiexec.exe ");

		wcscpy_s(szParams, L" /i ");
		wcscat_s(szParams, m_szFilename); // full path to 'lip_ca-es.msi'
		wcscat_s(szParams, L" /qn");
	}
	else // Windows Vista and 7
	{	
		// Documentation: http://technet.microsoft.com/en-us/library/cc766010%28WS.10%29.aspx
		wcscpy_s(szParams, L" /i ca-ES /r /s /p ");
		wcscat_s(szParams, m_szFilename);
	
		GetSystemDirectory(lpkapp, MAX_PATH);
		wcscat_s(lpkapp, L"\\lpksetup.exe");
	}

	status = InProgress;
	g_log.Log(L"WindowsLPIAction::Execute '%s' with params '%s'", lpkapp, szParams);
	m_runner->Execute(lpkapp, szParams, m_OSVersion->IsWindows64Bits());
}

// After the language package is installed we need to set Catalan as default language
// The key PreferredUILanguagesPending did not work as expected
void WindowsLPIAction::_setDefaultLanguage()
{
	if (m_OSVersion->GetVersion() == WindowsXP)
		return;
	
	// Sets the language for the default user
	if (m_registry->OpenKey(HKEY_CURRENT_USER, L"Control Panel\\Desktop", true) == TRUE)
	{
		m_registry->SetString(L"PreferredUILanguages", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"WindowsLPIAction::_setDefaultLanguage current user done");
	}

	// Sets the language for all users
	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\MUI\\Settings", true) == TRUE)
	{
		m_registry->SetMultiString(L"PreferredUILanguages", L"ca-ES");
		m_registry->Close();
		g_log.Log(L"WindowsLPIAction::_setDefaultLanguage all users done");
	}
}

ActionStatus WindowsLPIAction::GetStatus()
{
	if (status == InProgress)
	{
		if (m_runner->IsRunning())
			return InProgress;

		if (_isLangPackInstalled()) {
			status = Successful;
			_setDefaultLanguage();
		}
		else {
			status = FinishedWithError;			
		}
		
		g_log.Log(L"WindowsLPIAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

bool WindowsLPIAction::IsRebootNeed()
{
	return status == Successful;
}

#define SPANISH_LOCALEID 0x0A
#define FRENCH_LOCALEID 0x0c

void WindowsLPIAction::CheckPrerequirements(Action * action) 
{
	LANGID langid;
	WORD primary;
	bool bLangOk;

	langid = m_win32I18N->GetSystemDefaultUILanguage();
	primary = PRIMARYLANGID(langid);
	
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		bLangOk = (primary == SPANISH_LOCALEID);
	}
	else
	{
		bLangOk = (primary == SPANISH_LOCALEID || primary == FRENCH_LOCALEID);
	}

	if (bLangOk == false)
	{
		_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_NOSPFR, szCannotBeApplied);
		g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Incorrect Windows base language found (langid %u)",
			(wchar_t* )langid);
		status = CannotBeApplied;
		return;
	}

	if (m_OSVersion->IsWindows64Bits() == false)
	{
		if (m_OSVersion->GetVersion() != WindowsXP && m_OSVersion->GetVersion() != WindowsVista && m_OSVersion->GetVersion() != Windows7)
		{
			_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDWIN, szCannotBeApplied);
			g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported Windows version",
				(wchar_t* )langid);
			status = CannotBeApplied;
			return;
		}
	} 
	else // 64 bits
	{
		if (m_OSVersion->GetVersion() != Windows7)
		{
			_getStringFromResourceIDName(IDS_WINDOWSLPIACTION_UNSUPPORTEDWIN, szCannotBeApplied);
			g_log.Log(L"WindowsLPIAction::CheckPrerequirements. Unsupported Windows version",
				(wchar_t* )langid);
			status = CannotBeApplied;
			return;
		}
	}
}


bool WindowsLPIAction::IsWindowsValidated() 
{		
	if (m_OSVersion->GetVersion() == WindowsXP)
	{
		DWORD dwservicePack;

		dwservicePack = m_OSVersion->GetServicePackVersion();
	
		if (HIWORD(dwservicePack) < 2)
		{
			g_log.Log(L"WindowsLPIAction::IsWindowsValidated. Old XP");
			return true;
		}
		return _isWindowsValidated();
	}
	else
	{
		g_log.Log(L"WindowsLPIAction::IsWindowsValidated. No XP");
		return true;
	}
}

#define FUNCTION_ID 0x1

// Strategy: only says that is validated if we can really confirm it
bool WindowsLPIAction::_isWindowsValidated()
{
	CLSID lcsid;
	IDispatch* disp;
	VARIANT dispRes;
	EXCEPINFO *pExcepInfo = NULL;
	unsigned int *puArgErr = NULL;
	bool bRslt;	

	CoInitialize(NULL);
	
	if (!SUCCEEDED(CLSIDFromString(L"{17492023-C23A-453E-A040-C7C580BBF700}", &lcsid)))
	{
		g_log.Log(L"WindowsLPIAction::IsWindowsValidated. CLSIDFromString failed, passed: 0");
		return false;
	}
	 
    if (!SUCCEEDED(CoCreateInstance(lcsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IDispatch), (void**)&disp)))
	{
		g_log.Log(L"WindowsLPIAction::IsWindowsValidated. CreateInstance failed, passed: 0");
		return false;
	}

	DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};

	HRESULT hr = disp->Invoke(FUNCTION_ID, IID_NULL,
                               LOCALE_SYSTEM_DEFAULT, 
                               DISPATCH_METHOD,
                               &dispparamsNoArgs, &dispRes,
                               pExcepInfo, puArgErr);

	disp->Release();

	bRslt = wcscmp(dispRes.bstrVal, L"0") == 0;
	g_log.Log(L"WindowsLPIAction::IsWindowsValidated. Result: '%s', passed %u", dispRes.bstrVal,  (wchar_t *)bRslt);
	return bRslt;
}

