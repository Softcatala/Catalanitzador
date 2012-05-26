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
#include <windows.h>

#include "PropertySheetUI.h"
#include "PropertyPageUI.h"

#define ID_APPLY_NOW	0x3021
#define ID_APPLY		0x3021

PropertySheetUI::PropertySheetUI()
{	
	setOkButton(false);
	setApplyButton(true);
	m_lpfnDefSheet = NULL;
	m_pfnDlgProc = s_sheetWndProc;	
	m_pCallback = NULL;
	m_pages = NULL;
	m_modeless = false;
}

int CALLBACK PropertySheetUI::s_sheetWndProc(HWND hWnd, UINT msg, WPARAM wParam,  LPARAM lParam)
{			
	PropertySheetUI *pThis = (PropertySheetUI*)GetWindowLong(hWnd, GWL_USERDATA);			
	
	switch(msg) 
	{
		case WM_SYSCOMMAND:  
		{
			if (!pThis->_onSysCommand(hWnd, wParam, lParam))
				return 0; // Already processed

			// Support the closing button
			if (wParam==SC_CLOSE)
			{
				if (!pThis->m_modeless)
				{
			   		SendMessage (hWnd, WM_COMMAND, IDCANCEL, 0L);
			   		return 0;
			   	}
			   	else			   
				{				
					pThis->destroy();
					return 0;
				}				
			}
			break;
		}
			
		
		case WM_DESTROY:
		{		
			if (!pThis->m_modeless)
				PostQuitMessage(0);

			pThis->cleanup ();

			return 0;
		} 

		case WM_COMMAND:
		{	
			if (!pThis->_onCommand(hWnd, wParam, lParam)) 	
				return 0; // Already processed

			if (LOWORD(wParam)==ID_APPLY)
			{
				PropertyPageUI* pPage;	
				unsigned int i= 0;
				
				for(i=0; i< pThis->m_vecPages.size();  i++)
				{			
					pPage = (PropertyPageUI*)pThis->m_vecPages.at(i);		
					pPage->_onApply();					
				}

				pThis->_onApply();

				pThis->m_nRslt=ID_APPLY;
				return 0;
			}
			
			
			if (LOWORD(wParam)==IDOK)
			{
				
				PropertyPageUI* pPage;	
				unsigned int i= 0;
				
				for(i=0; i< pThis->m_vecPages.size();  i++)
				{			
					pPage = (PropertyPageUI*)pThis->m_vecPages.at(i);
					pPage->_onOK();					
				}

				pThis->_onOK();
				pThis->m_nRslt=IDOK;

				if (!pThis->m_modeless)
					pThis->destroy();

				return 0;
			}				
				
			if (LOWORD(wParam)==IDCANCEL)
			{				
				bool result;
				wchar_t szMessage [MAX_LOADSTRING];
				wchar_t szCaption [MAX_LOADSTRING];

				LoadString(GetModuleHandle(NULL), IDS_DOYOUWANTOCANCEL, szMessage, MAX_LOADSTRING);
				LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

				result = (MessageBox(hWnd, szMessage, szCaption,
					MB_YESNO | MB_ICONQUESTION) == IDYES);

				if (result)
				{
					pThis->m_nRslt=IDCANCEL;
					pThis->destroy();
					PostQuitMessage(0);
				}
				return 0;
			}			

			break;
		}				
						
		default:
			break;
	}  
	
	return CallWindowProc(pThis->m_lpfnDefSheet, hWnd, msg, wParam, lParam);       	
}


PROPSHEETPAGE* PropertySheetUI::_buildPageArray()
{			
	PROPSHEETPAGE *pArPages = new PROPSHEETPAGE[m_vecPages.size()];
	PROPSHEETPAGE *pCurPage;
	pCurPage = pArPages;
	PropertyPageUI* pPage;
	
	int count = m_vecPages.size();
	int i= 0;
	for(i=0; i< count; pCurPage++, i++)
	{
		pPage = m_vecPages.at (i);
		memcpy (pCurPage,  pPage->getStruct(), sizeof(PROPSHEETPAGE));
	}
	
	return pArPages;	
}


void PropertySheetUI::addPage(PropertyPageUI* pPage)
{
	m_vecPages.push_back(pPage);
}


int PropertySheetUI::runModal(HINSTANCE hInstance, HWND hParent, LPWSTR pCaption)
{	
	MSG msg;	
	m_pages = _buildPageArray();	
		
	m_nRslt = IDCANCEL;
		
	memset (&m_psh, 0, sizeof(PROPSHEETHEADER));		
		
	m_psh.dwSize = sizeof(PROPSHEETHEADER);
	m_psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD;
	m_psh.hwndParent = hParent;
	m_psh.hInstance = hInstance;
	m_psh.hIcon = NULL;
	m_psh.pszIcon = NULL;	
	m_psh.nPages = m_vecPages.size();
    m_psh.nStartPage = 0;
	m_psh.ppsp = (LPCPROPSHEETPAGE) m_pages;
	m_psh.pfnCallback = m_pCallback;    
    
	if (m_pCallback)
		m_psh.dwFlags |= PSH_USECALLBACK;    	
    
	if (!m_bApplyButton)
		m_psh.dwFlags |= PSH_NOAPPLYNOW;
    
	m_psh.pszCaption = pCaption;
	m_psh.dwFlags |= PSH_MODELESS;	
	m_hWnd = (HWND)::PropertySheet(&m_psh);
	EnableWindow(m_psh.hwndParent, FALSE);

	// Set buttons for first page after creation
	PropertyPageUI* pPage = m_vecPages.at(0);
	pPage->_sendSetButtonsMessage();

	/* Subclassing */
	m_lpfnDefSheet = (WHICHPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);	
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pfnDlgProc);
		
	_onInitDialog();

	while (GetMessage(&msg, NULL, 0, 0))
	{				
		if(m_hWnd && PropSheet_IsDialogMessage(m_hWnd, &msg))
			continue;				
					
		TranslateMessage(&msg);
		DispatchMessage(&msg);		
	}	
	
	destroy();	
	return m_nRslt;
}


void PropertySheetUI::destroy(void)
{	
	if (::IsWindow(m_hWnd))
	{	
		if (!m_modeless)
			EnableWindow(m_psh.hwndParent, TRUE);		

		DestroyWindow(m_hWnd);
	}

	cleanup ();	
}

void PropertySheetUI::cleanup(void)
{	
	if (m_pages) {
		delete[] m_pages;
		m_pages = NULL;
	}	
}
