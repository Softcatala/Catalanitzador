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
#include <math.h>
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include "Authorization.h"
#include "Url.h"

FinishPropertyPageUI::FinishPropertyPageUI()
{
	m_uploadStatistics = NULL;
	m_hFont = NULL;
}

FinishPropertyPageUI::~FinishPropertyPageUI()
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	if (m_uploadStatistics != NULL)
	{
		delete m_uploadStatistics;
		m_uploadStatistics = NULL;
	}

	if (m_xmlFile.size() > 0)
	{
		DeleteFile(m_xmlFile.c_str());
	}
}

void FinishPropertyPageUI::_onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	wchar_t szURL[1024];
	wchar_t szText[MAX_LOADSTRING], szString[MAX_LOADSTRING];

	if (wParam == IDC_TWITTER_BUTTON)
	{
		wstring parameter;

		LoadString(GetModuleHandle(NULL), IDS_SOCIALNETWORKS_TWITTER, szString, MAX_LOADSTRING);
		swprintf_s(szText, szString, APPLICATON_WEBSITE);
		
		Url::EncodeParameter(wstring(szText), parameter);
		wcscpy_s(szURL, L"http://twitter.com/share?text=");
		wcscat_s(szURL, parameter.c_str());
		ShellExecute(NULL, L"open", szURL, NULL, NULL, SW_SHOWNORMAL);
	}

	if (wParam == IDC_FACEBOOK_BUTTON)
	{
		// See: http://developers.facebook.com/docs/share/
		swprintf_s(szURL, L"http://ca-es.facebook.com/sharer.php?u=%s", APPLICATON_WEBSITE);
		ShellExecute(NULL, L"open", szURL, NULL, NULL, SW_SHOWNORMAL);
	}

	if (wParam == IDC_GOOGLEPLUS_BUTTON)
	{
		// See: https://developers.google.com/+/plugins/+1button/		
		swprintf_s(szURL, L"http://plusone.google.com/_/+1/confirm?hl=ca&url=%s", APPLICATON_WEBSITE);
		ShellExecute(NULL, L"open", szURL, NULL, NULL, SW_SHOWNORMAL);
	}
}
void FinishPropertyPageUI::_saveToDisk()
{
	wchar_t szXML[MAX_PATH];

	GetTempPath(MAX_PATH, szXML);	
	wcscat_s(szXML, L"results.xml");
	m_xmlFile = szXML;
	m_serializer->SaveToFile(m_xmlFile);
}

void FinishPropertyPageUI::_onInitDialog()
{
	if (*m_pbSendStats)
	{
		m_uploadStatistics = new UploadStatistics(m_serializer);
		m_uploadStatistics->StartUploadThread();
	}

	m_levelProgressBar = GetDlgItem(getHandle(), IDC_LEVEL_PROGRESSBAR);
	m_hFont = Window::CreateBoldFont(getHandle());
	SendMessage(GetDlgItem (getHandle(), IDC_CONGRATULATIONS), WM_SETFONT, (WPARAM) m_hFont, TRUE);
	SendMessage(GetDlgItem (getHandle(), IDC_HELPSOCIALNETWORKS), WM_SETFONT, (WPARAM) m_hFont, TRUE);
	SendMessage(GetDlgItem (getHandle(), IDC_FEEDBACK), WM_SETFONT, (WPARAM) m_hFont, TRUE);

	_setProgressBarLevelAndPercentage();
	_saveToDisk();
}

void FinishPropertyPageUI::_setProgressBarLevelAndPercentage()
{
	Action* action;
	int doable, done;
	bool errors = false;
	wchar_t szText[128];
	
	doable = done = 0;
	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

		switch (action->GetStatus())
		{			
			case AlreadyApplied:
			case Successful:
				doable++;
				done++;
				break;
			case NotSelected:
				doable++;
				break;
			case FinishedWithError:
				doable++;
				errors = true;
				break;
			default:
				break;
		}
	}

	SendMessage(m_levelProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, doable));
	SendMessage(m_levelProgressBar, PBM_SETPOS, done, 0);

	// Percentage	
	float per = doable != 0 ? floor ((float) done/ (float)doable * 100.0f) : 0;
	swprintf_s(szText, L"%2.0f%%", per);
	SendMessage(GetDlgItem(getHandle(), IDC_LEVEL_PERCENTAGE), WM_SETTEXT, 0, (LPARAM) szText);

	if (errors)
		ShowWindow(GetDlgItem(getHandle(), IDC_PROCESSWASNOTFULLYSUCCESSFULL), SW_NORMAL);
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
	Authorization::RequestShutdownPrivileges();
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

