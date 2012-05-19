/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "HideApplicationWindow.h"

#include <map>
using namespace std;

#define TIMER_ID_XP 2015
#define DIALOG_BOX_ATOM 32770
 
static map <int, HideApplicationWindow*> s_timersIDsToObjs;

HideApplicationWindow::HideApplicationWindow()
{
	m_nTimerID = 0;
}

void HideApplicationWindow::Start()
{	
	m_nTimerID = SetTimer(NULL, TIMER_ID_XP, 50, _timerProc);
	s_timersIDsToObjs[m_nTimerID] = this;
	g_log.Log(L"HideApplicationWindow::Start");
}

void HideApplicationWindow::Stop()
{
	if (m_nTimerID == 0)
		return;

	KillTimer(NULL, m_nTimerID);
	s_timersIDsToObjs.erase(m_nTimerID);
	m_nTimerID = 0;
	g_log.Log(L"HideApplicationWindow::Stop");
}

void HideApplicationWindow::AddControlsID(int ids [])
{
	for (int i = 0; ids[i] != NULL; i++)
	{
		m_controlsID.push_back(ids[i]);
		m_controlsFounds.push_back(false);
	}
}

bool HideApplicationWindow::_findControls(HWND hWnd)
{
	HWND child = NULL;
	
	for (unsigned int i = 0; i < m_controlsFounds.size(); i++)
	{
		m_controlsFounds[i] = false;
	}

	while (true)
	{
		child = FindWindowEx(hWnd, child, NULL, NULL);

		if (child == NULL)
			break;

		int id = GetDlgCtrlID(child);
		for (unsigned int i = 0; i < m_controlsID.size(); i++)
		{
			if (m_controlsID.at(i) == id)
			{
				g_log.Log(L"HideApplicationWindow::_findControls. Found control %x,  hwnd '%u", (wchar_t*) id, (wchar_t*) hWnd);
				m_controlsFounds[i] = true;
				break;
			}
		}
	}

	for (unsigned int i = 0; i < m_controlsFounds.size(); i++)
	{
		if (m_controlsFounds[i] == false)
			return false;
	}
	return true;
}


VOID CALLBACK HideApplicationWindow::_timerProc(HWND hWndTimer, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{	
	HWND hWnd;
	map<int, HideApplicationWindow*>::iterator it;
	HideApplicationWindow* obj;

	it = s_timersIDsToObjs.find(idEvent);

	if (it == s_timersIDsToObjs.end())
		return;

	obj = it->second;
		
	hWnd = FindWindowEx(NULL,NULL, MAKEINTATOM(DIALOG_BOX_ATOM), NULL);

	while (hWnd)
	{
		if (obj->HideApplicationWindow::_findControls(hWnd))
		{
			wchar_t szText[2048];

			obj->Stop();
			GetWindowText(hWnd, szText, sizeof(szText));
			ShowWindow(hWnd,SW_HIDE);
			g_log.Log(L"HideApplicationWindow::_timerProc. Found window title '%s', hwnd '%u', sent WM_SHOWWINDOW message", szText,
				(wchar_t*) hWndTimer);
		}
		hWnd = FindWindowEx(NULL, hWnd, MAKEINTATOM(DIALOG_BOX_ATOM), NULL);
	}
}
