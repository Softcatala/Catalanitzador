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
#include "WindowsXPDialogCanceler.h"

#define TIMER_ID_XP 2014
#define DIALOG_BOX_ATOM 32770

#define CANCEL_ID 2
#define EXPLORE_ID 0X6E
#define COMBOBOX_ID 0X6B
#define STATIC_ID 0X6C
#define TEXT_ID 0X68

void WindowsXPDialogCanceler::Start()
{
	if (m_version.GetVersion() != WindowsXP)
		return;

	m_nTimerID = SetTimer(NULL, TIMER_ID_XP, 500, _timerWindowsXPProc);
}

void WindowsXPDialogCanceler::Stop()
{
	if (m_version.GetVersion() != WindowsXP)
		return;
	
	KillTimer(NULL, m_nTimerID);
}

bool WindowsXPDialogCanceler::_isXPDialogLocale(HWND hWnd)
{
	HWND child = NULL;	
	bool bFoundCombo, bCancel, bExplore;
	HWND hHwndCancel;

	bExplore = bFoundCombo = bCancel = false;
	while (true)
	{
		child = FindWindowEx(hWnd, child, NULL, NULL);

		if (child == NULL)
			break;

		switch (GetDlgCtrlID(child))
		{
			case COMBOBOX_ID:
				bFoundCombo = true;
				break;
			case CANCEL_ID:
				hHwndCancel = child;
				bCancel = true;
				break;
			case EXPLORE_ID:
				bExplore = true;
				break;
			default:
				break;
		}
	}

	if (bFoundCombo && bCancel && bExplore)
	{
		PostMessage(hHwndCancel, BM_CLICK, 0,0);
		g_log.Log(L"WindowsXPDialogCanceler::_isXPDialogLocale. Found Windows XP dialog and sent click event");
		return true;
	}

	return false;
}

bool WindowsXPDialogCanceler::_isXPDialogWinLPI(HWND hWnd)
{
	HWND child = NULL;	
	bool bFoundStatic, bFoundCancel, bFoundText;
	HWND hHwndCancel;

	bFoundStatic = bFoundCancel = bFoundText = false;
	while (true)
	{
		child = FindWindowEx(hWnd, child, NULL, NULL);

		if (child == NULL)
			break;

		switch (GetDlgCtrlID(child))
		{
			case STATIC_ID:
				bFoundStatic = true;
				break;
			case CANCEL_ID:
				hHwndCancel = child;
				bFoundCancel = true;
				break;
			case TEXT_ID:
				bFoundText = true;
				break;
			default:
				break;
		}
	}

	if (bFoundStatic && bFoundCancel && bFoundText)
	{
		PostMessage(hHwndCancel, BM_CLICK, 0,0);
		g_log.Log(L"WindowsXPDialogCanceler::_isXPDialogLPI. Found Windows XP dialog and sent click event");		
		return true;
	}

	return false;
}

VOID CALLBACK WindowsXPDialogCanceler::_timerWindowsXPProc(HWND hWndTimer, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{	
	HWND hWnd;
		
	hWnd = FindWindowEx(NULL,NULL, MAKEINTATOM(DIALOG_BOX_ATOM), NULL);

	while (hWnd)
	{
		if (_isXPDialogLocale(hWnd) || _isXPDialogWinLPI(hWnd))
		{
			break;
		}

		hWnd = FindWindowEx(NULL, hWnd, MAKEINTATOM(32770), NULL);
	}	
}

