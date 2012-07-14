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

#pragma once

#include <windows.h>
#include <commctrl.h>
#include <vector>

class PropertyPageUI;

using namespace std;

// HACK: forward declarations for subclassed controls
#ifdef STRICT   
#define WHICHPROC	WNDPROC
#else   
#define WHICHPROC	FARPROC
#endif

class PropertySheetUI
{
public:
	PropertySheetUI();		
	
public:
	
	int							runModal(HINSTANCE hInstance, HWND hParent, LPWSTR pCaption);	
	void 						addPage(PropertyPageUI* pPage);
	PROPSHEETPAGE* 				_buildPageArray(void);		
	virtual	void				_onInitDialog(){};
	virtual void 				destroy(void);
	virtual void 				cleanup(void);
 	HWND						getHandle(){return m_hWnd;}	
	bool						isAero() { return m_bIsAero;}
	static int CALLBACK			PropSheetProcedure(HWND hWnd, UINT uMsg, LPARAM lParam);
	
	int							m_nRslt;
	vector <PropertyPageUI *>	m_vecPages;
	HWND						m_hWnd;

private:	
	
	PROPSHEETHEADER				m_psh;	
	PFNPROPSHEETCALLBACK		m_pCallback;
	
	bool						m_bApplyButton;
	bool						m_bOkButton;	
	PROPSHEETPAGE*				m_pages;	
	bool						m_bIsAero;
	
};

