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

#include <stdafx.h>
#include <Commctrl.h>
#include "AboutBoxDlgUI.h"
#include "Version.h"

void AboutBoxDlgUI::Run(HWND hWnd)
{
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX),
	          hWnd, reinterpret_cast<DLGPROC>(DlgProc));
}

LRESULT AboutBoxDlgUI::DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
				wchar_t szDate [MAX_LOADSTRING], szTime [MAX_LOADSTRING], szVersion [MAX_LOADSTRING]; 
				wchar_t szResource [MAX_LOADSTRING], szString [MAX_LOADSTRING];
				HWND hWnd;
				
				hWnd = GetDlgItem(hWndDlg, IDC_CATALANITZADOR_VERSION);
			
				MultiByteToWideChar(CP_ACP, 0,  __DATE__, strlen ( __DATE__) + 1,
					  szDate, sizeof (szDate));

				MultiByteToWideChar(CP_ACP, 0,  __TIME__, strlen ( __TIME__) + 1,
					  szTime, sizeof (szTime));
				
				MultiByteToWideChar(CP_ACP, 0,  STRING_VERSION, strlen (STRING_VERSION) + 1,
				      szVersion, sizeof (szVersion));
				
				LoadString(GetModuleHandle(NULL), IDS_ABOUTDLG_VERSION, szResource, MAX_LOADSTRING);
				swprintf_s(szString, szResource, szVersion, szDate, szTime);
				SetWindowText(hWnd, szString);
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
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
			break;
		}

		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->code == NM_CLICK)
			{
				ShellExecute(NULL, L"open", APPLICATON_WEBSITE, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	return FALSE;
}

