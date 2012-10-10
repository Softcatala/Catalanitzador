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
#include "SystemRestore.h"

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

void ExtraSecTermsDlgUI::_onInitDialog()
{
	HWND hWnd;
	SystemRestore systemRestore;

	hWnd = GetDlgItem(getHandle(), IDC_WELCOME_ABOUTSECURITY);
	m_hFont = Window::CreateBoldFont(getHandle());
	SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hFont, TRUE);

	HANDLE handle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CHECKMARK), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);	
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);

	if (systemRestore.Init() == true)
	{
		CheckDlgButton(getHandle(), IDC_SYSTEMRESTORE, *m_pSystemRestore);
	}
	else
	{
		EnableWindow(GetDlgItem(getHandle(), IDC_SYSTEMRESTORE), FALSE);
	}	
}

void ExtraSecTermsDlgUI::_onCommand(WPARAM wParam, LPARAM lParam)			
{
	if (wParam == IDOK)
	{
		*m_pSystemRestore = IsDlgButtonChecked(getHandle(), IDC_SYSTEMRESTORE)==BST_CHECKED;
	}
}
