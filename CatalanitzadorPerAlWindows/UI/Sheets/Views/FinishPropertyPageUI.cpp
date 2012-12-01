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

FinishPropertyPageUI::FinishPropertyPageUI(FinishModel* finishModel)
{	
	m_hFont = NULL;
	m_model = finishModel;
}

FinishPropertyPageUI::~FinishPropertyPageUI()
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void FinishPropertyPageUI::_onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case IDC_TWITTER_BUTTON:
			m_model->OpenTwitter();
			break;
		case IDC_FACEBOOK_BUTTON:
			m_model->OpenFacebook();
			break;
		case IDC_GOOGLEPLUS_BUTTON:
			m_model->OpenGooglePlus();
			break;
		default:
			break;
	}
}

void FinishPropertyPageUI::_onInitDialog()
{
	m_levelProgressBar = GetDlgItem(getHandle(), IDC_LEVEL_PROGRESSBAR);
	m_hFont = Window::CreateBoldFont(getHandle());
	SendMessage(GetDlgItem (getHandle(), IDC_CONGRATULATIONS), WM_SETFONT, (WPARAM) m_hFont, TRUE);
	SendMessage(GetDlgItem (getHandle(), IDC_HELPSOCIALNETWORKS), WM_SETFONT, (WPARAM) m_hFont, TRUE);
	SendMessage(GetDlgItem (getHandle(), IDC_FEEDBACK), WM_SETFONT, (WPARAM) m_hFont, TRUE);

	_setProgressBarLevelAndPercentage();	
}

void FinishPropertyPageUI::_setProgressBarLevelAndPercentage()
{
	wchar_t szText[128];
	
	SendMessage(m_levelProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, 100));
	SendMessage(m_levelProgressBar, PBM_SETPOS, (WPARAM) m_model->GetCompletionPercentage(), 0);
	
	swprintf_s(szText, L"%2.0f%%", m_model->GetCompletionPercentage());
	SendMessage(GetDlgItem(getHandle(), IDC_LEVEL_PERCENTAGE), WM_SETTEXT, 0, (LPARAM) szText);

	if (m_model->HasErrors())
		ShowWindow(GetDlgItem(getHandle(), IDC_PROCESSWASNOTFULLYSUCCESSFULL), SW_NORMAL);
}

void FinishPropertyPageUI::_onFinish()
{
	bool result = false;

	if (m_model->IsRebootNeed())
	{		
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		LoadString(GetModuleHandle(NULL), IDS_REBOOTNEED, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

		result = (MessageBox(getHandle(), szMessage, szCaption,
			MB_YESNO | MB_ICONQUESTION) == IDYES);		
	}

	m_model->WaitForStatisticsToCompleteOrTimeOut();

	if (result)
	{
		m_model->Reboot();
	}
}

NotificationResult FinishPropertyPageUI::_onNotify(LPNMHDR hdr, int /*iCtrlID*/)
{
	if (hdr->code == NM_CLICK && hdr->idFrom == IDC_SYSLINK_SUGGESTIONS)
	{		
		m_model->OpenMailTo();
	}
	return ReturnFalse;
}
