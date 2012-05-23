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
#include "Actions.h"
#include "Version.h"
#include "ActiveX.h"
#include "Slideshow.h"
#include "StringConversion.h"
#include "Authorization.h"

CatalanitzadorPerAWindows::CatalanitzadorPerAWindows(HINSTANCE hInstance)
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

void CatalanitzadorPerAWindows::Run()
{
	if (_isAlreadyRunning() == true)
		return;

	_initLog();
	m_serializer.OpenHeader();

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
	wstring version;

	StringConversion::ToWideChar(string(STRING_VERSION), version);

	swprintf_s(szApp, L"CatalanitzadorPerAlWindows version %s", version.c_str());
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
		version == Windows8)
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
	uploadStatistics.StartUploadThread();

	g_log.Log (L"Show unsupported OS dialog");
	LoadString(GetModuleHandle(NULL), id, szMessage, MAX_LOADSTRING);
	LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
	MessageBox(NULL, szMessage, szCaption, MB_OK | MB_ICONINFORMATION);
	uploadStatistics.WaitBeforeExit();
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
	Actions actions;
	Slideshow slideshow;
	BOOL bSendStats = TRUE;
	BOOL bSystemRestore = TRUE;

	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.createPage(m_hInstance, IDD_WELCOME, NULL);
	welcome.SetSendStats(&bSendStats);
	welcome.SetSystemRestore(&bSystemRestore);
	sheet.addPage(&welcome);

	applications.createPage(m_hInstance, IDD_APPLICATIONS, NULL);
	applications.setParent(&sheet);
	applications.setPageButtons(NextBackButtons);
	vector <Action *> acts = actions.GetActions();
	applications.SetActions(&acts);	
	sheet.addPage(&applications);

	install.setParent(&sheet);
	install.setPageButtons(CancelButtonOnly);
	install.SetActions(&acts);
	install.SetSerializer(&m_serializer);
	install.SetSlideshow(&slideshow);
	install.SetSystemRestore(&bSystemRestore);
	install.createPage(m_hInstance, IDD_INSTALL, NULL);
	sheet.addPage(&install);

	finish.SetSerializer(&m_serializer);
	finish.setParent(&sheet);
	finish.SetActions(&acts);
	finish.setPageButtons(FinishButtonOnly);
	finish.SetSendStats(&bSendStats);
	finish.createPage(m_hInstance, IDD_FINISH, NULL);	
	sheet.addPage(&finish);
	slideshow.StartUnpackThread();

	sheet.runModal(m_hInstance, NULL, NULL);
}
