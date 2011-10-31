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

#include <stdio.h>
#include <vector>
using namespace std;

#define TIMER_ID 1714

void InstallPropertyPageUI::_onInitDialog()
{
	hTotalProgressBar = GetDlgItem (getHandle (), IDC_INSTALL_PROGRESS_TOTAL);
	hTaskProgressBar = GetDlgItem (getHandle (), IDC_INSTALL_PROGRESS_TASK);
	hDescription = GetDlgItem (getHandle (), IDC_INSTALL_DESCRIPTION);
	ShowWindowOnce = FALSE;
}

void InstallPropertyPageUI::_onShowWindow()
{
	if (ShowWindowOnce == FALSE)
	{
		SetTimer (getHandle (), TIMER_ID, 500, NULL);
		ShowWindowOnce = TRUE;
	}
}

void InstallPropertyPageUI::DownloadStatus(int total, int current, void *data)
{
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) data;

	SendMessage(pThis->hTaskProgressBar, PBM_SETRANGE32, 0, total);
	SendMessage(pThis->hTaskProgressBar, PBM_SETPOS, current, 0);	
}

void InstallPropertyPageUI::Execute(Action* action)
{
	wchar_t szString [MAX_LOADSTRING];
	wchar_t szText [MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_EXECUTING, szString, MAX_LOADSTRING);
	
	swprintf_s (szText, szString, action->GetName());
	SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szText);
	SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0);
	Window::ProcessMessages();
	action->Execute((ProgressStatus)DownloadStatus, this);
}

void InstallPropertyPageUI::Download(Action* action)
{
	wchar_t szString [MAX_LOADSTRING];
	wchar_t szText [MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_DOWNLOAD, szString, MAX_LOADSTRING);
	
	swprintf_s (szText, szString, action->GetName());
	SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szText);
	SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0);
	Window::ProcessMessages();
	action->Download((ProgressStatus)DownloadStatus, this);
}

void InstallPropertyPageUI::Completed ()
{
	wchar_t szString [MAX_LOADSTRING];
	int nCompleted = 255;

	SendMessage(hTaskProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(hTaskProgressBar, PBM_SETPOS, nCompleted, 0);	

	SendMessage(hTotalProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(hTotalProgressBar, PBM_SETPOS, nCompleted, 0);	

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_COMPLETED, szString, MAX_LOADSTRING);
	SendMessage (hDescription, WM_SETTEXT, 0, (LPARAM) szString);	
}

int InstallPropertyPageUI::_getSelectedActionsCount()
{
	Action* action;
	
	int cnt = 0;
	for (unsigned int i = 0; i < m_selectedActions->size(); i++)
	{
		action = m_selectedActions->at(i);
		if (action->GetStatus() == Selected)
			cnt++;
	}
	return cnt;
}

void InstallPropertyPageUI::_onTimer()
{
	Action* action;

	KillTimer(getHandle (), TIMER_ID);

	int cnt_selected = _getSelectedActionsCount();
	int cnt_total = m_selectedActions->size();

	SendMessage(hTotalProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, cnt_selected * 2));
	SendMessage(hTotalProgressBar, PBM_SETSTEP, 10, 0L); 
	
	m_serializer->StartAction();
	for (int i = 0; i < cnt_total; i++)
	{
		action = m_selectedActions->at(i);

		if (action->GetStatus () != Selected)
		{
			m_serializer->Serialize(action);
			continue;
		}

		Download (action);
		Execute (action);

		while (true)
		{
			ActionStatus status = action->GetStatus();

			if (status == Successful || status == FinishedWithError)
				break;

			Window::ProcessMessages();
			Sleep(500);
			Window::ProcessMessages();			
		}
		m_serializer->Serialize(action);
	}
	m_serializer->EndAction();
	Completed ();
}

