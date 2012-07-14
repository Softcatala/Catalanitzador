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
	m_handle = LoadLibrary(_T("Riched20.dll"));
}

ShowLicensesDlgUI::~ShowLicensesDlgUI()
{
	if (m_handle != NULL)
	{
		FreeLibrary(m_handle);
	}
}

void ShowLicensesDlgUI::_setLicenseTextForItem(int index)
{
	HWND hComboBox;
	HWND hRichEdit;
	Action* action;

	hComboBox = GetDlgItem(m_hWnd, IDC_LICENSE_COMBO);
	hRichEdit = GetDlgItem(m_hWnd, IDC_LICENSES_RICHEDIT);
	action = (Action *) SendMessage(hComboBox, CB_GETITEMDATA, index, 0);
	
	wstring license;
	action->GetLicense(license);
	SetWindowText(hRichEdit, license.c_str());

	POINT point;
	point.x = point.y = 0;
	SendMessage(hRichEdit, EM_SETSCROLLPOS, 0, (LPARAM)&point);
}

void ShowLicensesDlgUI::_fillActions()
{
	HWND hComboBox;
	Action* action;
	int index;
	
	hComboBox = GetDlgItem(m_hWnd, IDC_LICENSE_COMBO);

	for (unsigned int i = 0; i < m_actions->size (); i++)
	{
		action = m_actions->at(i);
		if (action->HasLicense() && action->GetStatus() == Selected)
		{
			index = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) action->GetName());
			SendMessage(hComboBox, CB_SETITEMDATA, index, (LPARAM) action);
		}
	}
}

void ShowLicensesDlgUI::_onInitDialog()
{	
	HWND hComboBox;

	_fillActions();

	hComboBox = GetDlgItem(m_hWnd, IDC_LICENSE_COMBO);
	SendMessage(hComboBox, CB_SETCURSEL, 0, 0);
	_setLicenseTextForItem(0);
}

void ShowLicensesDlgUI::_onCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wId = LOWORD(wParam);
	WORD wNotifyCode = HIWORD(wParam);

	if (wId == IDC_LICENSE_COMBO && wNotifyCode == CBN_SELCHANGE)
	{
		int item;

		item = SendDlgItemMessage(m_hWnd, IDC_LICENSE_COMBO, CB_GETCURSEL, 0, 0);
		_setLicenseTextForItem(item);
	}
}

