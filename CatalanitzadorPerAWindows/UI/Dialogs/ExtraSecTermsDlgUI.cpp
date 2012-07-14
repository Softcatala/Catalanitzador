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
#include "ExtraSecTermsDlgUI.h"

ExtraSecTermsDlgUI::ExtraSecTermsDlgUI()
{
	m_hFont = NULL;
}

ExtraSecTermsDlgUI::~ExtraSecTermsDlgUI()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void ExtraSecTermsDlgUI::Run(HWND hWnd)
{
	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EXTRASECTERMS),
	          hWnd, reinterpret_cast<DLGPROC>(DlgProc), (LPARAM) this);
}

LRESULT ExtraSecTermsDlgUI::DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
			HWND hWnd;
			ExtraSecTermsDlgUI* pThis = (ExtraSecTermsDlgUI*) lParam;

			hWnd = GetDlgItem(hWndDlg, IDC_WELCOME_ABOUTSECURITY);
			pThis->m_hFont = Window::CreateBoldFont(hWndDlg);
			SendMessage(hWnd, WM_SETFONT, (WPARAM) pThis->m_hFont, TRUE);

			HANDLE handle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CHECKMARK), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);	
			SendMessage(GetDlgItem(hWndDlg, IDC_BITMAPCHECK1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
			SendMessage(GetDlgItem(hWndDlg, IDC_BITMAPCHECK2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
			SendMessage(GetDlgItem(hWndDlg, IDC_BITMAPCHECK3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);

			Window::CenterWindow(hWndDlg);
			return TRUE;
		}

		case WM_SYSCOMMAND:  
		{
			// Support the closing button
			if (wParam==SC_CLOSE)
			{
				SendMessage (hWndDlg, WM_COMMAND, IDOK, 0L);
				return TRUE;
			}
			break;
		}

		case WM_COMMAND:
		{
			switch(wParam)
			{
			case IDOK:
				//ExtraSecTermsDlgUI* pThis = (ExtraSecTermsDlgUI*) lParam;
				//*pThis->m_pbSystemRestore = IsDlgButtonChecked(hWndDlg, IDC_SYSTEMRESTORE)==BST_CHECKED;
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

