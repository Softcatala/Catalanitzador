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
#include <Richedit.h>
#include "ShowLicensesDlgUI.h"

ShowLicensesDlgUI::ShowLicensesDlgUI()
{
	m_handle = NULL;
}

ShowLicensesDlgUI::~ShowLicensesDlgUI()
{
	if (m_handle != NULL)
	{
		FreeLibrary(m_handle);
	}
}

void ShowLicensesDlgUI::Run(HWND hWnd)
{
	if (m_handle == NULL)
	{
		m_handle = LoadLibrary(_T("Riched20.dll"));
	}

	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SHOWLICENSES),
	          hWnd, reinterpret_cast<DLGPROC>(_dlgProc), (LPARAM) this);
}

void ShowLicensesDlgUI::_setLicenseTextForItem(HWND hWndDlg, int index)
{
	HWND hComboBox;
	HWND hRichEdit;
	Action* action;

	hComboBox = GetDlgItem(hWndDlg, IDC_LICENSE_COMBO);
	hRichEdit = GetDlgItem(hWndDlg, IDC_LICENSES_RICHEDIT);
	action = (Action *) SendMessage(hComboBox, CB_GETITEMDATA, index, 0);
	
	wstring license;
	action->GetLicense(license);
	SetWindowText(hRichEdit, license.c_str());

	POINT point;
	point.x = point.y = 0;
	SendMessage(hRichEdit, EM_SETSCROLLPOS, 0, (LPARAM)&point);
}

LRESULT ShowLicensesDlgUI::_dlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
			ShowLicensesDlgUI* pThis = (ShowLicensesDlgUI*) lParam;
			HWND hComboBox;
			vector <Action *> * actions;
			
			hComboBox = GetDlgItem(hWndDlg, IDC_LICENSE_COMBO);
			actions = pThis->m_actions;

			for (unsigned int i = 0; i < actions->size (); i++)
			{
				Action* action = actions->at(i);
				if (action->HasLicense())
				{
					int index;
					
					index = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) action->GetName());
					SendMessage(hComboBox, CB_SETITEMDATA, index, (LPARAM) action);
				}
			}

			SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
			_setLicenseTextForItem(hWndDlg, 0);
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
			WORD wId = LOWORD(wParam);
			WORD wNotifyCode = HIWORD(wParam);
			
			switch(wId)
			{
				case IDOK:
					EndDialog(hWndDlg, 0);
					return TRUE;

				case IDC_LICENSE_COMBO:
				{
					if(wNotifyCode == CBN_SELCHANGE)
					{
						int item;

						item = SendDlgItemMessage(hWndDlg, IDC_LICENSE_COMBO, CB_GETCURSEL, 0, 0);
						_setLicenseTextForItem(hWndDlg, item);
					}
					break;
				}
			}
		}
	}
	return FALSE;
}