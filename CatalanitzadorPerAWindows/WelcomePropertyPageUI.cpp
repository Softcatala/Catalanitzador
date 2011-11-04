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

#include "stdafx.h"
#include "WelcomePropertyPageUI.h"

WelcomePropertyPageUI::WelcomePropertyPageUI()
{
	m_hFont = NULL;
}

WelcomePropertyPageUI::~WelcomePropertyPageUI()
{
	if (m_hFont)
	{
		DeleteObject (m_hFont);
		m_hFont = NULL;
	}
}

void WelcomePropertyPageUI::_createBoldFont(HWND hWnd)
{
	HFONT hFont;
	LOGFONT	logFont;

	hFont = (HFONT) SendMessage(hWnd, WM_GETFONT, NULL, NULL);
	GetObject(hFont, sizeof(LOGFONT), &logFont);
	logFont.lfWeight = FW_BOLD;
	m_hFont = CreateFontIndirect(&logFont);
}

void WelcomePropertyPageUI::_onInitDialog()
{
	HWND hWnd;

	hWnd = GetDlgItem (getHandle(), IDC_WELCOME_TOAPP);
	_createBoldFont (hWnd);

	SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hFont, TRUE);

	SendMessage(GetDlgItem (getHandle(), IDC_WELCOME_ABOUTSECURITY),
		WM_SETFONT, (WPARAM) m_hFont, TRUE);

	hWnd = GetDlgItem (getHandle(), IDC_WELCOME_TOAPP);
	CheckDlgButton(getHandle(), IDC_SENDRESULTS, TRUE);
	SetFocus(getHandle());
}

bool WelcomePropertyPageUI::_onNext()
{
	*m_pbSendStats = IsDlgButtonChecked(getHandle(),IDC_SENDRESULTS)==BST_CHECKED;
	return true;
}