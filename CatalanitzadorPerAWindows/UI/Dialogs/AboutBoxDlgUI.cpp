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
#include <commctrl.h>
#include "AboutBoxDlgUI.h"
#include "Version.h"
#include "StringConversion.h"

void AboutBoxDlgUI::_onInitDialog()
{
	wstring date, time, version;
	wchar_t szResource [MAX_LOADSTRING], szString [MAX_LOADSTRING];
	HWND hWnd;
	
	hWnd = GetDlgItem(m_hWnd, IDC_CATALANITZADOR_VERSION);

	StringConversion::ToWideChar(string(__DATE__), date);
	StringConversion::ToWideChar(string(__TIME__), time);
	StringConversion::ToWideChar(string(STRING_VERSION), version);			
	
	LoadString(GetModuleHandle(NULL), IDS_ABOUTDLG_VERSION, szResource, MAX_LOADSTRING);
	swprintf_s(szString, szResource, version.c_str(), date.c_str(), time.c_str());
	SetWindowText(hWnd, szString);
}

int AboutBoxDlgUI::_onNotify(LPNMHDR hdr, int /*iCtrlID*/)
{
	if (hdr->code == NM_CLICK)
	{
		ShellExecute(NULL, L"open", APPLICATON_WEBSITE, NULL, NULL, SW_SHOWNORMAL);
	}
	return 0;
}
