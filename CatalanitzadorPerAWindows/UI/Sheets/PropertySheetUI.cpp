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
#include "ConfigurationInstance.h"

#define ID_APPLY_NOW	0x3021
#define ID_APPLY		0x3021

PropertySheetUI::PropertySheetUI()
{
	m_pages = NULL;	
	m_bIsAero = ConfigurationInstance::Get().GetAeroEnabled();
}

PROPSHEETPAGE* PropertySheetUI::_buildPageArray()
{			
	PROPSHEETPAGE *pArPages = new PROPSHEETPAGE[m_vecPages.size()];
	PROPSHEETPAGE *pCurPage;
	pCurPage = pArPages;
	PropertyPageUI* pPage;
	
	int count = m_vecPages.size();	
	for(int i=0; i< count; pCurPage++, i++)
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

// If we ever have more than property sheet per application we can
// create a static map to map the instance to the object
static PropertySheetUI* g_currentObject = NULL;

int CALLBACK PropertySheetUI::PropSheetProcedure(HWND hWnd, UINT uMsg, LPARAM lParam)
{
	if (uMsg == PSCB_INITIALIZED)
	{		
		g_currentObject->m_hWnd = hWnd;				
		g_currentObject->_onInitDialog();
	}
	return 0;
}

int PropertySheetUI::runModal(HINSTANCE hInstance, HWND hParent, LPWSTR pCaption)
{
	m_pages = _buildPageArray();
	g_currentObject = this;
		
	memset (&m_psh, 0, sizeof(PROPSHEETHEADER));
	m_psh.dwSize = sizeof(PROPSHEETHEADER);
	m_psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD;

	if (isAero())
		m_psh.dwFlags |= PSH_AEROWIZARD;

	m_psh.hwndParent = hParent;
	m_psh.hInstance = hInstance;
	m_psh.hIcon = NULL;
	m_psh.pszIcon = NULL;	
	m_psh.nPages = m_vecPages.size();
    m_psh.nStartPage = 0;
	m_psh.ppsp = (LPCPROPSHEETPAGE) m_pages;
	m_psh.pfnCallback = PropSheetProcedure;
	m_psh.dwFlags |= PSH_USECALLBACK;
        
	m_psh.pszCaption = pCaption;
	return PropertySheet(&m_psh);	
}

void PropertySheetUI::cleanup(void)
{	
	if (m_pages) 
	{
		delete[] m_pages;
		m_pages = NULL;
	}
}
