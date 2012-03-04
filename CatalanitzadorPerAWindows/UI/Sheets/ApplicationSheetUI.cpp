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
#include "ApplicationSheetUI.h"
#include "Windows.h"
#include "AboutBoxDlgUI.h"

#define IDM_ABOUTBOX 0x0010

ApplicationSheetUI::ApplicationSheetUI()
{
	m_hIcon = NULL;
}

ApplicationSheetUI::~ApplicationSheetUI()
{
	if (m_hIcon != NULL)
	{
		DestroyIcon(m_hIcon);
	}
}

void ApplicationSheetUI::_onInitDialog()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CATALANITZADORPERAWINDOWS));
	SendMessage(getHandle(), WM_SETICON, TRUE, (LPARAM) m_hIcon);
	SendMessage(getHandle(), WM_SETICON, FALSE, (LPARAM) m_hIcon);

	Window::CenterWindow(getHandle());
	_setAboutBoxMenu();
}

void ApplicationSheetUI::_setAboutBoxMenu()
{
	HMENU hMenu = GetSystemMenu(getHandle(), FALSE);

	if (hMenu == NULL)
		return;
	
	wchar_t szResource[MAX_LOADSTRING];

	AppendMenu(hMenu, MF_SEPARATOR, 0, MF_STRING);
	LoadString(GetModuleHandle(NULL), IDS_MENU_ABOUTBOX, szResource, MAX_LOADSTRING);
	AppendMenu(hMenu, MF_STRING, IDM_ABOUTBOX, szResource);
}

int ApplicationSheetUI::_onSysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDM_ABOUTBOX)
	{
		AboutBoxDlgUI aboutDlg;
		aboutDlg.Run(hWnd);
		return 0;
	}
	return 1;
}

