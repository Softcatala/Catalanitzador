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
#include "PropertyPageUI.h"
#include "PropertySheetUI.h"
#include "OSVersion.h"

PropertyPageUI::PropertyPageUI()
{
	m_pfnDlgProc = s_pageWndProc;
	m_PageButtons = DefaultButtons;

	OSVersion osversion;
	m_bIsAero = osversion.GetVersion() != WindowsXP;

#ifdef FORCE_NON_AERO
	m_bIsAero = false;
#endif
}

PropertyPageUI::~PropertyPageUI()
{
}


int CALLBACK PropertyPageUI::s_pageWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	NMHDR* pNMHDR;
	
	switch(msg) 
	{
		case WM_TIMER:
		{
			PropertyPageUI* pThis  = (PropertyPageUI *) GetWindowLong (hWnd, DWL_USER);
			if (pThis != NULL)
			{			
				pThis->_onTimer();
			}
			break;
		}

		case WM_SHOWWINDOW:
		{			
			PropertyPageUI* pThis  = (PropertyPageUI *) GetWindowLong (hWnd, DWL_USER);
			if (pThis != NULL)
			{			
				pThis->_onShowWindow();
			}
			break;
		}

		case WM_INITDIALOG:
		{	
			PROPSHEETPAGE*	pStruct = (PROPSHEETPAGE*)lParam;  		
			PropertyPageUI *pThis = (PropertyPageUI *)pStruct->lParam;
			SetWindowLong(hWnd,DWL_USER, pStruct->lParam);
			pThis->m_hWnd = hWnd;
			pThis->_sendSetButtonsMessage();
			pThis->_onInitDialog();
			return 0;
		}		

		case WM_NOTIFY:
		{
			PropertyPageUI *pThis = (PropertyPageUI *)GetWindowLong(hWnd,DWL_USER);
			pNMHDR = (NMHDR*)lParam;

			switch (pNMHDR->code)
			{
			case PSN_KILLACTIVE:				
				pThis->_onKillActive();
				break;
			case PSN_WIZNEXT:				
				if (pThis->_onNext() == false)
				{
					SetWindowLong(hWnd, DWL_MSGRESULT, -1);
					return TRUE;
				}
			case PSN_WIZFINISH:
				pThis->_onFinish();				
				break;
			case PSN_SETACTIVE:
				pThis->_sendSetButtonsMessage();
				break;
			case PSN_QUERYCANCEL:

				bool cancel_exit;
				wchar_t szMessage [MAX_LOADSTRING];
				wchar_t szCaption [MAX_LOADSTRING];

				LoadString(GetModuleHandle(NULL), IDS_DOYOUWANTOCANCEL, szMessage, MAX_LOADSTRING);
				LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);
				
				cancel_exit = (MessageBox(hWnd, szMessage, szCaption, MB_YESNO | MB_ICONQUESTION) != IDYES);
				SetWindowLong(hWnd, DWL_MSGRESULT, cancel_exit ? TRUE: FALSE);

				if (cancel_exit == false)
				{
					PostQuitMessage(0);
				}
				return TRUE;
			default:
				break;
			}

			if (pThis != NULL)
			{
				switch (pThis->_onNotify((LPNMHDR) lParam, wParam))
				{
					case ReturnTrue:						
						return TRUE;
					case ReturnFalse:
						return FALSE;
					case CallDefProc:
						break;
				}
				break;
			}
		}		
		
		case WM_COMMAND:
		{
			PropertyPageUI *pThis = (PropertyPageUI *)GetWindowLong(hWnd,DWL_USER);

			if (pThis)
				pThis->_onCommand(hWnd, wParam, lParam); 	

			return 0; // Already processed
		}				
						
		default:
			break;
	}
	
	return 0;
}

void PropertyPageUI::createPage(HINSTANCE hInstance, WORD wRscID, WORD wRscIDAero, LPWSTR pTitle)
{	
	LPCTSTR lpTemplate = isAero() ? MAKEINTRESOURCE(wRscIDAero) : MAKEINTRESOURCE(wRscID);
		
	m_page.dwSize = sizeof(PROPSHEETPAGE);
	m_page.dwFlags = PSP_DEFAULT;
	m_page.hInstance = hInstance;
	m_page.hIcon  = NULL;
	m_page.pszIcon  = NULL;	
	m_page.pszTemplate = lpTemplate;
	m_page.pfnDlgProc = m_pfnDlgProc;
	m_page.lParam = (LPARAM) this;
	m_page.pfnCallback = NULL;
	m_page.pcRefParent  = NULL;
	m_page.pszHeaderTitle = pTitle;

	if (m_page.pszHeaderTitle != NULL)
		m_page.dwFlags |= PSP_USEHEADERTITLE; 

	CreatePropertySheetPage(&m_page);
}

void PropertyPageUI::_sendSetButtonsMessage()
{
	if (m_PageButtons == DefaultButtons)
		return;

	int buttons = 0;

	switch (m_PageButtons)
	{
		case NextButton:
			buttons = PSWIZB_NEXT;
			break;
		case BackButton:
			buttons = PSWIZB_BACK;
			break;
		case NextBackButtons:
			buttons = PSWIZB_NEXT | PSWIZB_BACK;
			break;
		case FinishButtonOnly:
			buttons = PSWIZB_FINISH;
			EnableWindow(GetDlgItem (getParent()->getHandle(), IDCANCEL), FALSE);
			break;
	}

	SendMessage (getParent()->getHandle(), PSM_SETWIZBUTTONS, 0, buttons);
}
