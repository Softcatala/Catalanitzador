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

#include <stdafx.h>
#include "CatalanitzadorPerAWindows.h"
#include "PropertyPageUI.h"
#include "InstallPropertyPageUI.h"
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ApplicationsPropertyPageUI.h"
#include "WelcomePropertyPageUI.h"
#include "ApplicationSheetUI.h"
#include "ActiveX.h"
#include "Slideshow.h"
#include "Authorization.h"
#include "ConfigurationInstance.h"
#include "guid.h"
#include "Registry.h"
#include "ApplicationVersion.h"
#include "CatalanitzadorUpdateAction.h"
#include "Runner.h"

#define COMMAND_DELIMITER L' '
#define VERSION_PARAMETER L"/version"
#define NORUNNING_PARAMETER L"/NoRunningCheck"
#define USEAEROLOOK_PARAMETER L"/UseAeroLook"
#define USECLASSICLOOK_PARAMETER L"/UseClassicLook"
#define HELP_PARAMETER L"/Help"

CatalanitzadorPerAWindows::CatalanitzadorPerAWindows(HINSTANCE hInstance)
: m_actions(m_downloadManager)
{
	m_hInstance = hInstance;
	m_hEvent = NULL;
	m_bRunningCheck = true;

	NORUNNING_PARAMETER_LEN = wcslen(NORUNNING_PARAMETER);
	VERSION_PARAMETER_LEN = wcslen(VERSION_PARAMETER);
	USEAEROLOOK_PARAMETER_LEN = wcslen(USEAEROLOOK_PARAMETER);
	USECLASSICLOOK_PARAMETER_LEN = wcslen(USECLASSICLOOK_PARAMETER);
	HELP_PARAMETER_LEN = wcslen(HELP_PARAMETER);
}

CatalanitzadorPerAWindows::~CatalanitzadorPerAWindows()
{
	g_log.Close();

	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);
}

#define HELP_TEXT L"Sintaxis d'ús:\n\n\
/version:X.Y.Z - Fa creure al Catalanitzador que és la versió indicada \n\
/NoRunningCheck- No comprovis si ja s'està executant \n\
/UseAeroLook - Usa l'aspecte Aero \n\
/UseClassicLook - Usa l'aspecte clàssic \n"

void CatalanitzadorPerAWindows::_createCatalanitzadorUpdateAction(wstring version)
{
	if (version.size() == 0)
		return;

	CatalanitzadorUpdateAction* action;
	
	action = (CatalanitzadorUpdateAction *)m_actions.GetActionFromID(CatalanitzadorUpdate);
	action->SetVersion(version);
	action->SetStatus(Successful);
}

void CatalanitzadorPerAWindows::_processCommandLine(wstring commandLine)
{
	wchar_t* pch;
	
	pch = (wchar_t*) commandLine.c_str();

	while (*pch != NULL)
	{
		if (_wcsnicmp(pch, NORUNNING_PARAMETER, NORUNNING_PARAMETER_LEN) == 0)
		{
			m_bRunningCheck = false;
			pch += NORUNNING_PARAMETER_LEN;

			if (wcslen(pch) > 0)
			{
				wchar_t version[32];
				wchar_t* start, *end;

				pch++;
				start = pch;
				end = wcschr(start, L' ');
				if (end == NULL) end = pch +  wcslen(start);

				wcsncpy_s(version, start, end - start);
				_createCatalanitzadorUpdateAction(version);

				pch = end;
			}			

		} else if (_wcsnicmp(pch, USEAEROLOOK_PARAMETER, USEAEROLOOK_PARAMETER_LEN) == 0)
		{
			ConfigurationInstance::Get().SetAeroEnabled(true);
			pch += USEAEROLOOK_PARAMETER_LEN;
		} else if (_wcsnicmp(pch, USECLASSICLOOK_PARAMETER, USECLASSICLOOK_PARAMETER_LEN) == 0)
		{
			ConfigurationInstance::Get().SetAeroEnabled(false);
			pch += USECLASSICLOOK_PARAMETER_LEN;
		}
		else if (_wcsnicmp(pch, HELP_PARAMETER, HELP_PARAMETER_LEN) == 0)
		{
			MessageBox(NULL, HELP_TEXT, NULL, NULL);
			exit(0);
		}
		else if (_wcsnicmp(pch, VERSION_PARAMETER, VERSION_PARAMETER_LEN) == 0)
		{
			wchar_t version[32];
			wchar_t* start, *end;
			
			pch += VERSION_PARAMETER_LEN;			

			if (wcslen(pch) > 0)
			{
				pch++;
				start = pch;
				end = wcschr(start, L' ');
				if (end == NULL) end = pch +  wcslen(start);

				wcsncpy_s(version, start, end - start);
				ConfigurationInstance::Get().SetVersion(ApplicationVersion(version));

				pch = end;
			}
		}
		else
			pch++;
	}
}

void CatalanitzadorPerAWindows::Run(wstring commandLine)
{
	_processCommandLine(commandLine);

	if (_isAlreadyRunning() == true)
		return;

	_initLog();

	Registry registry;
	Guid guid(&registry);
	guid.Get();

	m_serializer.OpenHeader();
	m_serializer.Serialize(&guid);
	guid.Store();
	
	if (_supportedOS() == true && _hasAdminPermissionsDialog() == true)
	{
		OleInitialize(0);
		AXRegister();
		_createWizard();
	}
}

void CatalanitzadorPerAWindows::_initLog()
{
	wchar_t szApp[1024];

	swprintf_s(szApp, L"CatalanitzadorPerAlWindows version %s", STRING_VERSION);
	g_log.CreateLog(L"CatalanitzadorPerAlWindows.log",szApp);
	
	wchar_t szOSInfo [2048];
	m_osVersion.GetLogInfo(szOSInfo, sizeof (szOSInfo));
	g_log.Log(szOSInfo);
}

bool CatalanitzadorPerAWindows::_supportedOS()
{
	int id;
	OperatingVersion version;

	version = m_osVersion.GetVersion();

	if (version == Windows2000 ||
		version == WindowsXP64_2003)
	{
		id = IDS_NOTSUPPORTEDOS;
	}
	else
	{
		return true;
	}

	wchar_t szMessage [MAX_LOADSTRING];
	wchar_t szCaption [MAX_LOADSTRING];
	UploadStatistics uploadStatistics(&m_serializer);

	m_serializer.CloseHeader();
	uploadStatistics.Start();

	g_log.Log (L"Show unsupported OS dialog");
	LoadString(GetModuleHandle(NULL), id, szMessage, MAX_LOADSTRING);
	LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
	MessageBox(NULL, szMessage, szCaption, MB_OK | MB_ICONINFORMATION);
	uploadStatistics.Wait();
	return false;
}

bool CatalanitzadorPerAWindows::_hasAdminPermissionsDialog()
{
	bool hasAdmin;

	hasAdmin = Authorization::UserHasAdminPermissions();

	if (hasAdmin == false)
	{	
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		g_log.Log (L"Show not admin user dialog");
		LoadString(GetModuleHandle(NULL), IDS_NOUSERADMIN, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
		MessageBox(NULL, szMessage, szCaption, MB_OK | MB_ICONINFORMATION);
	}
	return hasAdmin;
}

bool CatalanitzadorPerAWindows::_isAlreadyRunning()
{
	if (m_bRunningCheck == false)
		return false;

    m_hEvent = CreateEvent(NULL, TRUE, FALSE, L"Catalanitzador");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
        return true;
    }
    return false;
}

void CatalanitzadorPerAWindows::_createWizard()
{
	ApplicationSheetUI sheet;
	WelcomePropertyPageUI welcome;
	ApplicationsPropertyPageUI applications;
	InstallPropertyPageUI install;
	FinishPropertyPageUI finish;
	SystemRestore systemRestore;
	Slideshow slideshow;
	BOOL bSendStats = TRUE;
	BOOL bSystemRestore = TRUE;

#if _DEBUG
	bSendStats = FALSE;
	bSystemRestore = FALSE;
#endif

	if (systemRestore.Init() == false)
		bSystemRestore = FALSE;

	vector <Action *> acts = m_actions.GetActions();
	
	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.SetSendStats(&bSendStats);
	welcome.SetSystemRestore(&bSystemRestore);
	welcome.SetActions(&acts);
	welcome.createPage(m_hInstance, IDD_WELCOME, IDD_WELCOME_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_WELCOME));
	sheet.addPage(&welcome);

	applications.createPage(m_hInstance, IDD_APPLICATIONS, IDD_APPLICATIONS_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_APPLICATIONS));
	applications.setParent(&sheet);
	applications.setPageButtons(NextBackButtons);
	
	applications.SetActions(&acts);
	sheet.addPage(&applications);

	install.setParent(&sheet);
	install.setPageButtons(CancelButtonOnly);
	install.SetActions(&acts);
	install.SetSerializer(&m_serializer);
	install.SetSlideshow(&slideshow);
	install.SetSystemRestore(&bSystemRestore);
	install.createPage(m_hInstance, IDD_INSTALL, IDD_INSTALL_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_INSTALL));
	sheet.addPage(&install);

	finish.SetSerializer(&m_serializer);
	finish.setParent(&sheet);	
	finish.setPageButtons(FinishButtonOnly);
	finish.SetSendStats(&bSendStats);
	finish.SetActions(&acts);
	finish.createPage(m_hInstance, IDD_FINISH, IDD_FINISH_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_FINISH));	
	sheet.addPage(&finish);
	slideshow.Start();

	sheet.runModal(m_hInstance, NULL, (LPWSTR)IDS_WIZARD_TITLE);
}
