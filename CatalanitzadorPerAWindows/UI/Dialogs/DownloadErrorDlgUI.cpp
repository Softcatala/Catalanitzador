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

#include <stdafx.h>
#include <Commctrl.h>
#include "DownloadErrorDlgUI.h"

DownloadErrorDlgUI::DownloadErrorDlgUI(wstring action_name)
{	
	m_action_name = action_name;
}

bool DownloadErrorDlgUI::Run(HWND hWnd)
{
	return DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DOWNLOADERROR),
	          hWnd, reinterpret_cast<DLGPROC>(DlgProc), (LPARAM) this) == IDOK;
}

LRESULT DownloadErrorDlgUI::DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
				DownloadErrorDlgUI* pThis = (DownloadErrorDlgUI*) lParam;
				wchar_t szMsg[MAX_LOADSTRING], szString[MAX_LOADSTRING];
				
				LoadString(GetModuleHandle(NULL), IDS_DOWNLOADERROR_ACTION, szString, MAX_LOADSTRING);
				swprintf_s(szMsg, szString, pThis->m_action_name.c_str());
				SetWindowText(GetDlgItem (hWndDlg, IDC_STATIC_ACTION), szMsg);
				Window::CenterWindow(hWndDlg);
				return TRUE;
		}

		case WM_SYSCOMMAND:  
		{
			// Support the closing button
			if (wParam==SC_CLOSE)
			{
				SendMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
				return TRUE;
			}
			break;
		}

		case WM_COMMAND:
		{
			switch(wParam)
			{
			case IDOK:
				EndDialog(hWndDlg, IDOK);
				return TRUE;
			case IDCANCEL:
				EndDialog(hWndDlg, IDCANCEL);
				return TRUE;
			}
			break;
		}

	}

	return FALSE;
}

