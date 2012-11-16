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
#include "CatalanitzadorPerAlWindows.h"
#include "PropertyPageUI.h"
#include "InstallPropertyPageUI.h"
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ApplicationsPropertyPageUI.h"
#include "WelcomePropertyPageUI.h"
#include "ApplicationSheetUI.h"
#include "ActiveX.h"
#include "Authorization.h"
#include "guid.h"
#include "Registry.h"
#include "ApplicationVersion.h"
#include "Runner.h"
#include "CommandLine.h"
#include "SystemRestore.h"
#include "HttpFormInet.h"
#include "ApplicationsModel.h"

CatalanitzadorPerAWindows::CatalanitzadorPerAWindows(HINSTANCE hInstance)
: m_actions(&m_downloadManager)
{
	m_hInstance = hInstance;
	m_hEvent = NULL;
}

CatalanitzadorPerAWindows::~CatalanitzadorPerAWindows()
{
	g_log.Close();

	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);
}

void CatalanitzadorPerAWindows::Run(wstring commandLine)
{
	Registry registry;
	CommandLine commandLineProcessor(&m_actions);

	_initLog();
	commandLineProcessor.Process(commandLine);
	
	if (commandLineProcessor.GetRunningCheck() == true)
	{
		if (_isAlreadyRunning() == true)
			return;	
	}

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
		version == WindowsXP64_2003 ||
		version == Windows2012)
	{
		id = IDS_NOTSUPPORTEDOS;
	}
	else
	{
		return true;
	}

	wchar_t szMessage [MAX_LOADSTRING];
	wchar_t szCaption [MAX_LOADSTRING];
	HttpFormInet httpFormInet;

	UploadStatisticsThread uploadStatistics(&httpFormInet, &m_serializer, false);

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
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, L"Catalanitzador");
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
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
	ApplicationsModel applicationModel;
	ApplicationsPropertyPageUI applications(&applicationModel);
	InstallPropertyPageUI install;
	FinishPropertyPageUI finish;
	SystemRestore sysRestore;
	bool bSendStats = true;
	int systemRestore = TRUE;
	bool bDialectVariant = false;
	bool bShowSecDlg = false;

#if _DEBUG
	bSendStats = false;
	systemRestore = TRUE;
#endif

	if (sysRestore.Init() == false)
		systemRestore = -1;

	vector <Action *> acts = m_actions.GetActions();
	
	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.SetSendStats(&bSendStats);
	welcome.SetSystemRestore(&systemRestore);
	welcome.SetActions(&m_actions);
	welcome.SetShowSecDlg(&bShowSecDlg);
	welcome.createPage(m_hInstance, IDD_WELCOME, IDD_WELCOME_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_WELCOME));
	sheet.addPage(&welcome);

	applicationModel.SetActions(&acts);
	applicationModel.SetView(&applications);	
	applications.createPage(m_hInstance, IDD_APPLICATIONS, IDD_APPLICATIONS_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_APPLICATIONS));
	applications.setParent(&sheet);
	applications.setPageButtons(NextBackButtons);
	applications.SetActions(&acts);
	applications.SetDialectVariant(&bDialectVariant);
	sheet.addPage(&applications);

	install.setParent(&sheet);
	install.setPageButtons(CancelButtonOnly);
	install.SetActions(&acts);
	install.SetSerializer(&m_serializer);
	install.SetSystemRestore(&systemRestore);
	install.SetDialectVariant(&bDialectVariant);
	install.SetShowSecDlg(&bShowSecDlg);
	install.StartSlideShowUnpack();
	install.createPage(m_hInstance, IDD_INSTALL, IDD_INSTALL_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_INSTALL));
	sheet.addPage(&install);

	finish.SetSerializer(&m_serializer);
	finish.setParent(&sheet);
	finish.setPageButtons(FinishButtonOnly);
	finish.SetSendStats(&bSendStats);
	finish.SetActions(&acts);
	finish.createPage(m_hInstance, IDD_FINISH, IDD_FINISH_AERO, MAKEINTRESOURCE(IDS_WIZARD_HEADER_FINISH));	
	sheet.addPage(&finish);

	sheet.runModal(m_hInstance, NULL, (LPWSTR)IDS_WIZARD_TITLE);
}
