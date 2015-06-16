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
#include "ActiveX.h"
#include "Authorization.h"
#include "guid.h"
#include "Registry.h"
#include "CommandLine.h"
#include "HttpFormInet.h"
#include "SilentInstallation.h"
#include "UIInstallation.h"

#define RETURN_OK 0
#define RETURN_ERROR 1


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

bool CatalanitzadorPerAWindows::_warnImpersonateUser()
{	
	Authorization authorization;

	if (authorization.IsRunningElevatedWithOtherUser(&m_osVersion))
	{
		wchar_t szMsg[MAX_LOADSTRING], szCaption [MAX_LOADSTRING], szText[MAX_LOADSTRING];		

		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_WARNING_IMPERSONATED_USER, szMsg, MAX_LOADSTRING);

		swprintf_s(szText, szMsg, authorization.GetCurrentUser().c_str(), authorization.GetLoggedUser().c_str(),
			authorization.GetLoggedUser().c_str());

		MessageBox(NULL, szText, szCaption, MB_OK | MB_ICONINFORMATION);
		return true;
	}
	return false;
}

int CatalanitzadorPerAWindows::Run(wstring commandLine)
{
	bool impersonated;
	Registry registry;
	ApplicationExecutor applicationExecutor;
	CommandLine commandLineProcessor(applicationExecutor.GetActionsObject());

	_initLog();
	commandLineProcessor.Process(commandLine);

	if (commandLineProcessor.GetRunningCheck() == true)
	{
		if (_isAlreadyRunning() == true)
			return RETURN_ERROR;
	}

	Guid guid(&registry);
	guid.Get();

	m_serializer.OpenHeader();
	m_serializer.Serialize(&guid);
	guid.Store();
	
	if (_supportedOS() == false || _hasAdminPermissionsDialog() == false)
		return RETURN_ERROR;

	impersonated = _warnImpersonateUser();

	OleInitialize(0);

	applicationExecutor.SetSerializer(&m_serializer);
	Option optionShowSecDlg(OptionSilentInstall, commandLineProcessor.GetSilent());
	applicationExecutor.SetOption(optionShowSecDlg);

	Option optionShowImpersonatedDlg(OptionShowImpersonatedDlg, impersonated);
	applicationExecutor.SetOption(optionShowImpersonatedDlg);

	if (commandLineProcessor.GetSilent())
	{
		SilentInstallation silentInstallation;
		silentInstallation.Run(applicationExecutor);
	}
	else
	{		
		AXRegister();
		UIInstallation uiInstallation;
		uiInstallation.Run(m_hInstance, applicationExecutor);
	}

	return applicationExecutor.HasErrors() ?  RETURN_ERROR :  RETURN_OK;
}

void CatalanitzadorPerAWindows::_initLog()
{
	wchar_t szApp[1024];

	swprintf_s(szApp, L"CatalanitzadorPerAlWindows version %s", STRING_VERSION);
	g_log.CreateLogInTempDirectory(L"CatalanitzadorPerAlWindows.log",szApp);
	
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
