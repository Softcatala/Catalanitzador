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
#include "PropertySheetUI.h"
#include "OSVersion.h"
#include "DownloadErrorDlgUI.h"
#include "WindowsXPDialogCanceler.h"
#include "Inspectors.h"
#include "ActiveX.h"
#include <exdisp.h>

#define TIMER_ID 1714

void InstallPropertyPageUI::_openURLInIE()
{
	IWebBrowser2* wb = 0;
	HWND hX = GetDlgItem(getHandle (),IDC_INTERNETEXPLORER);

	SendMessage(hX,AX_INPLACE, 1,0);
	SendMessage(hX,AX_QUERYINTERFACE,(WPARAM)&IID_IWebBrowser2,(LPARAM)&wb);
	if (wb)
	{		
		m_slideshow.Wait();
		wb->Navigate((BSTR)m_slideshow.GetURL().c_str(),0,0,0,0);
		wb->Release();
	}
}

void InstallPropertyPageUI::_onInitDialog()
{
	hTotalProgressBar = GetDlgItem (getHandle(), IDC_INSTALL_PROGRESS_TOTAL);
	hTaskProgressBar = GetDlgItem (getHandle(), IDC_INSTALL_PROGRESS_TASK);
	hDescription = GetDlgItem (getHandle(), IDC_INSTALL_DESCRIPTION);
	ShowWindowOnce = FALSE;
	_openURLInIE();
}

void InstallPropertyPageUI::_onShowWindow()
{
	if (ShowWindowOnce == FALSE)
	{
		SetTimer(getHandle(), TIMER_ID, 500, NULL);
		ShowWindowOnce = TRUE;
	}
}

const float BYTES_TO_MEGABYTES = 1024*1024;

struct DownloadData
{
	InstallPropertyPageUI* pThis;
	Action* action;
};

void InstallPropertyPageUI::_downloadStatus(int total, int current, void *data)
{
	DownloadData* downloadData = (DownloadData *) data;	
	wchar_t szString[MAX_LOADSTRING];
	wchar_t szText[MAX_LOADSTRING];

	SendMessage(downloadData->pThis->hTaskProgressBar, PBM_SETRANGE32, 0, total);
	SendMessage(downloadData->pThis->hTaskProgressBar, PBM_SETPOS, current, 0);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_DOWNLOAD, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, downloadData->action->GetName(), ((float)current) / BYTES_TO_MEGABYTES, ((float)total) / BYTES_TO_MEGABYTES);
	SendMessage(downloadData->pThis->hDescription, WM_SETTEXT, 0, (LPARAM) szText);
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
	DownloadData downloadData;

	if (action->IsDownloadNeed() == false)
		return true;

	_setTaskMarqueeMode(false);

	Window::ProcessMessages();

	bDownload = true;
	while (bDownload)
	{
		downloadData.pThis = this;
		downloadData.action = action;
		if (action->Download((ProgressStatus)_downloadStatus, &downloadData) == false)
		{
			DownloadErrorDlgUI dlgError(action->GetName());
			if (dlgError.Run(getHandle()) != IDOK)
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
	return bError == false;
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

void InstallPropertyPageUI::_systemRestore(SystemRestoreThread& systemRestore)
{
	wchar_t szText[MAX_LOADSTRING];
		
	_setTaskMarqueeMode(true);

	LoadString(GetModuleHandle(NULL), IDS_CREATINGSYSTEMRESTORE, szText, MAX_LOADSTRING);
	SendMessage(hDescription, WM_SETTEXT, 0, (LPARAM) szText);

	systemRestore.Start();
	systemRestore.Wait();
	SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0);
}

void InstallPropertyPageUI::_onTimer()
{
	SystemRestoreThread systemRestore;
	Action* action;
	WindowsXPDialogCanceler dialogCanceler;
	int cnt;

	KillTimer(getHandle(), TIMER_ID);
	
	dialogCanceler.Start();

	_updateSelectedActionsCounts();

	cnt = m_downloads + m_selActions;

	if (m_selActions > 0 && *m_pbSystemRestore)
		cnt++;	

	SendMessage(hTotalProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, cnt));
	SendMessage(hTotalProgressBar, PBM_SETSTEP, 10, 0L);
	
	if (m_selActions > 0 && *m_pbSystemRestore)
	{
		_systemRestore(systemRestore);
	}

	m_serializer->StartAction();
	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

		if (action->GetStatus() != Selected)
		{
			m_serializer->Serialize(action);
			continue;
		}

		if (_download(action) == true)
		{
			_execute(action);
			_waitExecutionComplete(action);
		}
		else
		{
			action->SetStatus(NotSelected);
		}

		SendMessage(hTotalProgressBar, PBM_DELTAPOS, 1, 0); // 'Execute' completed
		m_serializer->Serialize(action);
		Window::ProcessMessages();
	}

	dialogCanceler.Stop();
	m_serializer->EndAction();

	Inspectors inspectors;
	inspectors.Execute();
	inspectors.Serialize(m_serializer->GetStream());

	m_serializer->CloseHeader();
	_completed();

	if (m_selActions > 0 && *m_pbSystemRestore)
	{		
		systemRestore.End();
	}
}
