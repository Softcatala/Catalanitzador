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
#include "InstallPropertyPageUI.h"
#include "Actions.h"
#include "Action.h"
#include "PropertySheetUI.h"
#include "Window.h"
#include "resource.h"
#include "OSVersion.h"
#include "DownloadErrorDlgUI.h"

#include <stdio.h>
#include <vector>
using namespace std;

#define TIMER_ID 1714

void InstallPropertyPageUI::_onInitDialog()
{
	hTotalProgressBar = GetDlgItem (getHandle(), IDC_INSTALL_PROGRESS_TOTAL);
	hTaskProgressBar = GetDlgItem (getHandle(), IDC_INSTALL_PROGRESS_TASK);
	hDescription = GetDlgItem (getHandle(), IDC_INSTALL_DESCRIPTION);
	ShowWindowOnce = FALSE;	
}

void InstallPropertyPageUI::_onShowWindow()
{
	if (ShowWindowOnce == FALSE)
	{
		SetTimer (getHandle(), TIMER_ID, 500, NULL);
		ShowWindowOnce = TRUE;
	}
}

void InstallPropertyPageUI::_downloadStatus(int total, int current, void *data)
{
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) data;

	SendMessage(pThis->hTaskProgressBar, PBM_SETRANGE32, 0, total);
	SendMessage(pThis->hTaskProgressBar, PBM_SETPOS, current, 0);	
}

void InstallPropertyPageUI::_execute(Action* action)
{
	wchar_t szString [MAX_LOADSTRING];
	wchar_t szText [MAX_LOADSTRING];

	_setTaskMarqueeMode(true);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_EXECUTING, szString, MAX_LOADSTRING);
	swprintf_s (szText, szString, action->GetName());
	SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szText);	

	Window::ProcessMessages();
	action->Execute();
}

bool InstallPropertyPageUI::_download(Action* action)
{
	bool bDownload;
	bool bError = false;
	wchar_t szString [MAX_LOADSTRING];
	wchar_t szText [MAX_LOADSTRING];

	if (action->IsDownloadNeed() == false)
		return false;

	_setTaskMarqueeMode(false);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_DOWNLOAD, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, action->GetName());
	SendMessage(hDescription, WM_SETTEXT, 0, (LPARAM) szText);

	Window::ProcessMessages();

	bDownload = true;
	while (bDownload)
	{
		if (action->Download((ProgressStatus)_downloadStatus, this) == false)
		{
			DownloadErrorDlgUI dlgError(action->GetName());
			if (dlgError.Run(getHandle()) == false)
			{
				bDownload = false;
				bError = true;
			}
		}
		else
		{
			bDownload = false;
		}
	}

	SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0);
	return bError;
}

void InstallPropertyPageUI::_completed()
{
	wchar_t szString [MAX_LOADSTRING];
	int nCompleted = 255;

	_setTaskMarqueeMode(false);

	SendMessage(hTaskProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(hTaskProgressBar, PBM_SETPOS, nCompleted, 0);

	SendMessage(hTotalProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(hTotalProgressBar, PBM_SETPOS, nCompleted, 0);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_COMPLETED, szString, MAX_LOADSTRING);
	SendMessage(hDescription, WM_SETTEXT, 0, (LPARAM) szString);
	
	// Enable Wizard next button
	SendMessage(getParent()->getHandle(), PSM_SETWIZBUTTONS, 0, PSWIZB_NEXT);
}

void InstallPropertyPageUI::_updateSelectedActionsCounts()
{
	Action* action;

	m_selActions = m_downloads = 0;

	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);
		if (action->GetStatus() == Selected)
		{
			m_selActions++;
			if (action->IsDownloadNeed())
			{
				m_downloads++;
			}
		}
	}
}

void InstallPropertyPageUI::_windowsXPAsksCDWarning()
{
	OSVersion version;

	if (version.GetVersion() != WindowsXP)
		return;

	bool bShow = false;
	Action* action;
	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

		if (action->GetStatus() != Selected)
			continue;

		if (action->GetID() == WindowsLPI || action->GetID() == ConfigureLocale)
		{
			bShow = true;
			break;
		} 
		else
			break;
	}

	if (bShow)
	{
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];
		LoadString(GetModuleHandle(NULL), IDS_WINDOWSXP_ASKSCD, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
		MessageBox(getParent()->getHandle(), szMessage, szCaption, MB_OK | MB_ICONINFORMATION);
	}
}

void InstallPropertyPageUI::_setTaskMarqueeMode(bool enable)
{
	LONG_PTR style = GetWindowLongPtr(hTaskProgressBar, GWL_STYLE);

	if (enable)
	{
		SetWindowLongPtr(hTaskProgressBar, GWL_STYLE, style | PBS_MARQUEE);
		SendMessage(hTaskProgressBar, PBM_SETMARQUEE, TRUE, 0);
	}
	else
	{
		SendMessage(hTaskProgressBar, PBM_SETMARQUEE, FALSE, 0);
		SetWindowLongPtr(hTaskProgressBar, GWL_STYLE, style &~ PBS_MARQUEE);
	}
}

void InstallPropertyPageUI::_waitExecutionComplete(Action* action)
{
	while (true)
	{
		ActionStatus status = action->GetStatus();

		if (status == Successful || status == FinishedWithError)
			break;

		Window::ProcessMessages();
		Sleep(50);
		Window::ProcessMessages();
	}
}

void InstallPropertyPageUI::_onTimer()
{
	Action* action;

	KillTimer(getHandle(), TIMER_ID);

	_windowsXPAsksCDWarning();

	_updateSelectedActionsCounts();

	SendMessage(hTotalProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, m_downloads + m_selActions));
	SendMessage(hTotalProgressBar, PBM_SETSTEP, 10, 0L);

	m_serializer->StartAction();
	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

		if (action->GetStatus() != Selected)
		{
			m_serializer->Serialize(action);
			continue;
		}

		if (_download(action) == false)
		{
			_execute(action);
			_waitExecutionComplete(action);
		}
		else
		{
			action->SetStatus(FinishedWithError);
		}

		SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0); // 'Execute' completed
		m_serializer->Serialize(action);
		Window::ProcessMessages();
	}
	m_serializer->EndAction();
	m_serializer->CloseHeader();
	_completed();
}

