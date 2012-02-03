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
	if (_supportedOS() == true && _hasAdminPermissionsDialog() == true)
	{
		_createWizard();
	}
}

void CatalanitzadorPerAWindows::_initLog()
{
	wchar_t szApp [1024];
	wchar_t szVersion [256];

	MultiByteToWideChar(CP_ACP, 0,  STRING_VERSION, strlen (STRING_VERSION) + 1,
                  szVersion, sizeof (szVersion));

	swprintf_s(szApp, L"CatalanitzadorPerAlWindows version %s", szVersion);
	g_log.CreateLog(L"CatalanitzadorPerAlWindows.log",szApp);
	
	wchar_t szOSInfo [2048];
	m_osVersion.GetLogInfo(szOSInfo, sizeof (szOSInfo));
	g_log.Log(szOSInfo);
}

bool CatalanitzadorPerAWindows::_supportedOS()
{
	int id;

	if (m_osVersion.GetVersion() == Windows2000 ||
		(m_osVersion.GetVersion() == WindowsXP64_2003))
	{
		id = IDS_NOTSUPPORTEDOS;
	}
	else
	{
		return true;
	}

	wchar_t szMessage [MAX_LOADSTRING];
	wchar_t szCaption [MAX_LOADSTRING];
	UploadStatistics uploadStatistics (m_serializer.GetStream());

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

	hasAdmin = _hasAdminPermissions();

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

bool CatalanitzadorPerAWindows::_hasAdminPermissions()
{	
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	if (!AllocateAndInitializeSid(&NtAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0, &AdministratorsGroup))
	{
		return false;
	}
	
	BOOL IsInAdminGroup = FALSE;
	if(!CheckTokenMembership(NULL, AdministratorsGroup, &IsInAdminGroup))
	{	
		IsInAdminGroup = FALSE;
	}	
	FreeSid(AdministratorsGroup);
	return IsInAdminGroup == TRUE ? true : false;
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
	BOOL bSendStats = TRUE;

	welcome.setParent(&sheet);
	welcome.setPageButtons(NextButton);
	welcome.createPage(m_hInstance, IDD_WELCOME, NULL);
	welcome.SetSendStats(&bSendStats);
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
	install.createPage(m_hInstance, IDD_INSTALL, NULL);
	sheet.addPage(&install);

	finish.SetSerializer(&m_serializer);
	finish.setParent(&sheet);
	finish.SetActions(&acts);
	finish.setPageButtons(FinishButtonOnly);
	finish.SetSendStats(&bSendStats);
	finish.createPage(m_hInstance, IDD_FINISH, NULL);	
	sheet.addPage(&finish);

	sheet.runModal(m_hInstance, NULL, NULL);
}
