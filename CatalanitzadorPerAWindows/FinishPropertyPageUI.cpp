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
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"

FinishPropertyPageUI::FinishPropertyPageUI()
{
	m_uploadStatistics = NULL;
	m_hFont = NULL;
}

FinishPropertyPageUI::~FinishPropertyPageUI()
{
	if (m_hFont != NULL)
	{
		DeleteObject (m_hFont);
		m_hFont = NULL;
	}
}

void FinishPropertyPageUI::_onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	wchar_t szURL[1024];
	wchar_t* szText = L"He catalanitzat el meu PC! Fes-ho tu també a: http://www.softcatala.org/wiki/Rebost:Catalanitzador";

	if (wParam == IDC_TWITTER_BUTTON)
	{
		wcscpy_s(szURL, L"http://twitter.com/share?text=");
		wcscat_s(szURL, szText);
		ShellExecute(NULL, L"open", szURL, NULL, NULL, SW_SHOWNORMAL);
	}

	if (wParam == IDC_FACEBOOK_BUTTON)
	{
		// See: http://developers.facebook.com/docs/share/
		wcscpy_s(szURL, L"http://www.facebook.com/sharer.php?u=http://www.softcatala.org/wiki/Rebost:Catalanitzador&t=");
		wcscat_s(szURL, szText);		
		ShellExecute(NULL, L"open", szURL, NULL, NULL, SW_SHOWNORMAL);
	}	
}


void FinishPropertyPageUI::_onInitDialog()
{
	if (*m_pbSendStats)
	{
		m_uploadStatistics = new UploadStatistics(m_serializer->GetStream());
		m_uploadStatistics->StartUploadThread();
	}

	m_hFont = Window::CreateBoldFont(getHandle());
	SendMessage(GetDlgItem (getHandle(), IDC_CONGRATULATIONS),	WM_SETFONT, (WPARAM) m_hFont, TRUE);
}

bool FinishPropertyPageUI::_isRebootNeed()
{	
	Action* action;

	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

		if (action->IsRebootNeed())
		{
			return true;
		}
	}	
	return false;
}

void FinishPropertyPageUI::_onFinish()
{	
	if (_isRebootNeed())
	{
		bool result;
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		LoadString(GetModuleHandle(NULL), IDS_REBOOTNEED, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

		result = (MessageBox(getHandle(), szMessage, szCaption,
			MB_YESNO | MB_ICONQUESTION) == IDYES);

		if (result)
		{
			_shutdown();
		}
	}	
	if (m_uploadStatistics!= NULL)
		m_uploadStatistics->WaitBeforeExit();
}

void FinishPropertyPageUI::_shutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
	}
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

