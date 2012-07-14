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
#include "DlgUI.h"

int DlgUI::Run(HWND hWnd)
{
	return DialogBoxParam(GetModuleHandle(NULL), GetResourceTemplate(), hWnd, reinterpret_cast<DLGPROC>(DlgProc), (LPARAM) this);
}

LRESULT DlgUI::DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
			DlgUI* dlgUI = (DlgUI*) lParam;

			dlgUI->m_hWnd = hWndDlg;
			dlgUI->_onInitDialog();
			SetWindowLongPtr(hWndDlg,GWL_USERDATA, lParam);
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
			DlgUI* pThis  = (DlgUI *) GetWindowLongPtr(hWndDlg, GWL_USERDATA);
			pThis->_onCommand(wParam, lParam);

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

		case WM_NOTIFY:
		{
			DlgUI* pThis  = (DlgUI *) GetWindowLongPtr(hWndDlg, GWL_USERDATA);
			return pThis->_onNotify((LPNMHDR) lParam, wParam);
		}
	}

	return FALSE;
}
