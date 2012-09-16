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
#include "SystemRestore.h"
#include "PropertyPageUI.h"
#include "PropertySheetUI.h"
#include "ExtraSecTermsDlgUI.h"
#include "AboutBoxDlgUI.h"
#include "ConfigurationInstance.h"
#include "DownloadNewVersionDlgUI.h"

WelcomePropertyPageUI::WelcomePropertyPageUI()
{
	m_hFont = NULL;
}

WelcomePropertyPageUI::~WelcomePropertyPageUI()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void WelcomePropertyPageUI::_setTransparentBitmaps()
{	
	HANDLE handle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CHECKMARK), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);	
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK1), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
	SendMessage(GetDlgItem(getHandle(), IDC_BITMAPCHECK3), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);

	handle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HORIZONTAL_LOGO), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);	
	SendMessage(GetDlgItem(getHandle(), IDC_HORIZONTAL_LOGO), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) handle);
}

void WelcomePropertyPageUI::_initPropertySheet()
{
	HWND hWnd;

	hWnd = GetParent(getHandle());
	SetFocus(getHandle());
	SendMessage(hWnd, PSM_SETWIZBUTTONS, (WPARAM)0, (LPARAM)PSWIZB_NEXT);
	Window::CenterWindow(hWnd);

	m_configurationDownload.Start();
}

void WelcomePropertyPageUI::_onInitDialog()
{
	if (ConfigurationInstance::Get().GetAeroEnabled() == false)
	{
		HWND hWnd = GetDlgItem(getHandle(), IDC_WELCOME_TOAPP);
		m_hFont = Window::CreateBoldFont(hWnd);
		SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hFont, TRUE);
	}

	CheckDlgButton(getHandle(), IDC_SENDRESULTS, *m_pbSendStats);
	_setTransparentBitmaps();
	_initPropertySheet();
}

bool WelcomePropertyPageUI::_doesUserWantToUpdate()
{
	wchar_t szMessage [MAX_LOADSTRING];
	wchar_t szCaption [MAX_LOADSTRING];		

	LoadString(GetModuleHandle(NULL), IDS_WANT_TOUSE_NEWVERSION, szMessage, MAX_LOADSTRING);
	LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

	return MessageBox(getHandle(), szMessage, szCaption, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

void WelcomePropertyPageUI::_updateCatalanitzadorAction(Action* catalanitzadorAction)
{
	catalanitzadorAction->CheckPrerequirements(NULL);

	if (catalanitzadorAction->IsNeed() == true)
	{
		if (_doesUserWantToUpdate())
		{
			DownloadNewVersionDlgUI downloadNewVersionDlgUI(m_pActions->GetActionFromID(CatalanitzadorUpdate));
			if (downloadNewVersionDlgUI.Run(getHandle()) == IDCANCEL)
			{
				catalanitzadorAction->SetStatus(NotSelected);
			}
		}		
		else
		{
			catalanitzadorAction->SetStatus(NotSelected);			
		}	
	}	
}

bool WelcomePropertyPageUI::_onNext()
{
	Action* catalanitzadorAction = m_pActions->GetActionFromID(CatalanitzadorUpdate);

	// Disable Next button after has been click to prevent user clicking twice
	SendMessage(getParent()->getHandle(), PSM_SETWIZBUTTONS, 0, 0);
	Window::ProcessMessages();

	if (catalanitzadorAction->GetStatus() != Successful)
	{
		_updateCatalanitzadorAction(catalanitzadorAction);
	}
	
	m_pActions->CheckPrerequirements();
	*m_pbSendStats = IsDlgButtonChecked(getHandle(),IDC_SENDRESULTS)==BST_CHECKED;
	return true;
}

NotificationResult WelcomePropertyPageUI::_onNotify(LPNMHDR hdr, int /*iCtrlID*/)
{
	if (hdr->code == NM_CLICK)
    {
		switch (hdr->idFrom)
		{
			case IDC_SYSLINK_ABOUT:	
			{
				AboutBoxDlgUI aboutDlg;
				aboutDlg.Run(getHandle());
				break;
			}
			case IDC_SYSLINK_SECTERMS:
			{
				ExtraSecTermsDlgUI extraSecTermsDlgUI;
				extraSecTermsDlgUI.SetSystemRestore(m_pbSystemRestore);
				extraSecTermsDlgUI.Run(getHandle());
				break;
			}
			default:
				break;
		}
	}
	return ReturnFalse;
}
