/* 
 * Copyright (C) 2011-2018 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "CarregadorPerAlWindows.h"
#include "CatalanitzadorUpdateAction.h"
#include "SilentInstallation.h"
#include "DownloadNewVersionDlgUI.h"
#include "Actions.h"

#define RETURN_OK 0
#define RETURN_ERROR 1


CarregadorPerAlWindows::CarregadorPerAlWindows(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hEvent = NULL;
}

CarregadorPerAlWindows::~CarregadorPerAlWindows()
{
	g_log.Close();

	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);
}

void CarregadorPerAlWindows::_updateCatalanitzadorAction(Action* catalanitzadorAction)
{
	catalanitzadorAction->CheckPrerequirements(NULL);

	DownloadNewVersionDlgUI downloadNewVersionDlgUI(catalanitzadorAction);
	if (downloadNewVersionDlgUI.Run(NULL) == IDCANCEL)
	{
		catalanitzadorAction->SetStatus(NotSelected);
	}

	if (catalanitzadorAction->GetStatus () != Successful)
	{
		WCHAR* message = L"S'ha produït un error de connexió. Torneu-ho a provar més tard.\nAixò pot ser degut a un error de connexió o bé del servidor.";
		MessageBox(NULL, message, L"No es pot connectar amb el servidor", 0);
	}

}

int CarregadorPerAlWindows::Run(wstring commandLine)
{
	ApplicationExecutor applicationExecutor;

	_initLog();	
	
	if (_isAlreadyRunning() == true)
		return RETURN_ERROR;	

	DownloadManager downloadManager;
	Actions actions(&downloadManager);
	CatalanitzadorUpdateAction* catalanitzadorAction = (CatalanitzadorUpdateAction *)actions.GetActionFromID(CatalanitzadorUpdateActionID);	
	catalanitzadorAction->SetNoRunningCheck();
	catalanitzadorAction->SetCommandLine(commandLine);
	_updateCatalanitzadorAction(catalanitzadorAction);
	return TRUE;
}

void CarregadorPerAlWindows::_initLog()
{
	wchar_t szApp[1024];

	swprintf_s(szApp, L"CarregadorPerAlWindows version %s", STRING_VERSION);
	g_log.CreateLogInTempDirectory(L"CarregadorPerAlWindows.log",szApp);
}


bool CarregadorPerAlWindows::_isAlreadyRunning()
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, L"CarregadorPerAlWindows");
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
        return true;
    }
    return false;
}
