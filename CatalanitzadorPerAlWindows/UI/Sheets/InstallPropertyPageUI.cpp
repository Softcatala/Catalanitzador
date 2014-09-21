/* 
 * Copyright (C) 2011-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "DownloadErrorDlgUI.h"
#include "ActiveX.h"
#include <exdisp.h>

#define TIMER_ID 1714

const float BYTES_TO_MEGABYTES = 1024*1024;


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
	m_hTotalProgressBar = GetDlgItem(getHandle(), IDC_INSTALL_PROGRESS_TOTAL);
	m_hTaskProgressBar = GetDlgItem(getHandle(), IDC_INSTALL_PROGRESS_TASK);
	m_hDescription = GetDlgItem(getHandle(), IDC_INSTALL_DESCRIPTION);
	m_showWindowOnce = false;
	_openURLInIE();
}

void InstallPropertyPageUI::_onShowWindow()
{
	if (m_showWindowOnce == false)
	{
		SetTimer(getHandle(), TIMER_ID, 500, NULL);
		m_showWindowOnce = true;
	}
}

bool InstallPropertyPageUI::_downloadStatus(int total, int current, void *data)
{
	NotifyActionData* notifyActionData = (NotifyActionData *) data;
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI *) notifyActionData->pThis;
	wchar_t szString[MAX_LOADSTRING];
	wchar_t szText[MAX_LOADSTRING];

	pThis->_setTaskMarqueeMode(false);

	SendMessage(pThis->m_hTaskProgressBar, PBM_SETRANGE32, 0, total);
	SendMessage(pThis->m_hTaskProgressBar, PBM_SETPOS, current, 0);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_DOWNLOAD, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, notifyActionData->action->GetName(), ((float)current) / BYTES_TO_MEGABYTES, ((float)total) / BYTES_TO_MEGABYTES);
	SendMessage(pThis->m_hDescription, WM_SETTEXT, 0, (LPARAM) szText);
	Window::ProcessMessages();
	return true;
}

void InstallPropertyPageUI::_notifyExecutionStarts(NotifyActionData* notifyActionData)
{	
	wchar_t szString [MAX_LOADSTRING];
	wchar_t szText [MAX_LOADSTRING];
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) notifyActionData->pThis;

	pThis->_setTaskMarqueeMode(true);
	LoadString(GetModuleHandle(NULL), IDS_INSTALL_EXECUTING, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, notifyActionData->action->GetName());
	SendMessage(pThis->m_hDescription, WM_SETTEXT, 0, (LPARAM) szText);
	Window::ProcessMessages();
}

void InstallPropertyPageUI::_completed()
{
	wchar_t szString [MAX_LOADSTRING];
	int nCompleted = 255;

	_setTaskMarqueeMode(false);

	SendMessage(m_hTaskProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(m_hTaskProgressBar, PBM_SETPOS, nCompleted, 0);

	SendMessage(m_hTotalProgressBar, PBM_SETRANGE32, 0, nCompleted);
	SendMessage(m_hTotalProgressBar, PBM_SETPOS, nCompleted, 0);

	LoadString(GetModuleHandle(NULL), IDS_INSTALL_COMPLETED, szString, MAX_LOADSTRING);
	SendMessage(m_hDescription, WM_SETTEXT, 0, (LPARAM) szString);
	
	// Enable Wizard next button
	SendMessage(getParent()->getHandle(), PSM_SETWIZBUTTONS, 0, PSWIZB_NEXT);
}

// When Marquee mode is on the progress bar shows an animation for process that we cannot estimate duration
void InstallPropertyPageUI::_setTaskMarqueeMode(bool enable)
{
	if (m_marqueeMode.IsUndefined() == false && m_marqueeMode == enable)
		return;

	LONG_PTR style = GetWindowLongPtr(m_hTaskProgressBar, GWL_STYLE);

	if (enable)
	{
		SetWindowLongPtr(m_hTaskProgressBar, GWL_STYLE, style | PBS_MARQUEE);
		SendMessage(m_hTaskProgressBar, PBM_SETMARQUEE, TRUE, 0);
	}
	else
	{
		SendMessage(m_hTaskProgressBar, PBM_SETMARQUEE, FALSE, 0);
		SetWindowLongPtr(m_hTaskProgressBar, GWL_STYLE, style &~ PBS_MARQUEE);
	}

	m_marqueeMode = enable;
	Window::ProcessMessages();
}

void InstallPropertyPageUI::_systemRestore()
{
	wchar_t szText[MAX_LOADSTRING];
		
	_setTaskMarqueeMode(true);
	LoadString(GetModuleHandle(NULL), IDS_CREATINGSYSTEMRESTORE, szText, MAX_LOADSTRING);
	SendMessage(m_hDescription, WM_SETTEXT, 0, (LPARAM) szText);	
	Window::ProcessMessages();
}

void InstallPropertyPageUI::_notifyExecutionCompleted(NotifyActionData* notifyActionData)
{
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) notifyActionData->pThis;
	SendMessage(pThis->m_hTotalProgressBar, PBM_DELTAPOS, 1, 0);
	Window::ProcessMessages();
}

void InstallPropertyPageUI::_setTotalProgressBar()
{
	int totalSteps = m_model->GetTotalSteps();

	SendMessage(m_hTotalProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, totalSteps));
	SendMessage(m_hTotalProgressBar, PBM_SETSTEP, 10, 0L);
	Window::ProcessMessages();
}

bool InstallPropertyPageUI::_notifyDownloadError(NotifyActionData* notifyActionData)
{
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) notifyActionData->pThis;
	DownloadErrorDlgUI dlgError(notifyActionData->action->GetName());
	return dlgError.Run(pThis->getHandle()) != IDOK;
}

void InstallPropertyPageUI::_notifyDownloadCompleted(NotifyActionData* notifyActionData)
{
	InstallPropertyPageUI* pThis = (InstallPropertyPageUI*) notifyActionData->pThis;
	SendMessage(pThis->m_hTotalProgressBar, PBM_DELTAPOS, 1, 0);
	Window::ProcessMessages();
}

void InstallPropertyPageUI::_onTimer()
{
	KillTimer(getHandle(), TIMER_ID);

	m_model->PrepareStart();
	_setTotalProgressBar();

	_systemRestore();
	m_model->Start(_notifyExecutionStarts, _downloadStatus, _notifyDownloadError, _notifyDownloadCompleted, _notifyExecutionCompleted, this);
	_completed();
}
