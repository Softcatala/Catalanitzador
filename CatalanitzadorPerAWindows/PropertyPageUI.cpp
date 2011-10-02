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

PropertyPageUI::PropertyPageUI()
{
	m_pfnDlgProc = s_pageWndProc;	
	m_pParent = NULL;
	m_hdle = NULL;
}

PropertyPageUI::~PropertyPageUI()
{
	if (m_hdle)
		DestroyPropertySheetPage(m_hdle);	
}

void PropertyPageUI::setChanged (bool bChanged)
{
	HWND hWnd = GetParent(m_hWnd);
	SendMessage(hWnd, bChanged ? PSM_CHANGED : PSM_UNCHANGED, (WPARAM)m_hWnd, 0);	
}

int CALLBACK PropertyPageUI::s_pageWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	NMHDR* pNMHDR;
	
	switch(msg) 
	{
		case WM_INITDIALOG:
		{	
			PROPSHEETPAGE*	pStruct = (PROPSHEETPAGE*)lParam;  		
			PropertyPageUI *pThis = (PropertyPageUI *)pStruct->lParam;
			SetWindowLong(hWnd,DWL_USER,pStruct->lParam);
			pThis->m_hWnd = hWnd;
			pThis->_onInitDialog();
			return 0;
		}		

		case WM_NOTIFY:
		{
			PropertyPageUI *pThis = (PropertyPageUI *)GetWindowLong(hWnd,DWL_USER);

			if (pThis)
				pThis->_onNotify((LPNMHDR) lParam, wParam); 
			
			pNMHDR = (NMHDR*)lParam;					
			if (pNMHDR->code==PSN_KILLACTIVE)
			{
				PropertyPageUI *pThis = (PropertyPageUI *)GetWindowLong(hWnd,DWL_USER);
				pThis->_onKillActive();
			}
			break;
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
        
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void PropertyPageUI::createPage(HINSTANCE hInstance, WORD wRscID, LPWSTR pTitle)
{	
	LPCTSTR lpTemplate = MAKEINTRESOURCE(wRscID);	
	
	m_page.pszTitle = pTitle;
	
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

	m_hdle = CreatePropertySheetPage(&m_page);	
	
}
