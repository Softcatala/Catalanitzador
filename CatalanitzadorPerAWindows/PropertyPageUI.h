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

class PropertyPageUI
{
public:
	
	PropertyPageUI();
	~PropertyPageUI();
	
	void 						createPage(HINSTANCE hInstance, WORD wRscID, LPWSTR pTitle);	
	PROPSHEETPAGE*				getStruct(){return &m_page;}
	HWND						getHandle(){return m_hWnd;}	
	void						setDialogProc(DLGPROC pfnDlgProc){m_pfnDlgProc=pfnDlgProc;};	
	virtual	void				_onInitDialog(){};
	virtual	void				_onKillActive(){}; 	
	virtual	void				_onOK(){}; 		
	virtual	void				_onApply(){}; 
	virtual void				_onCommand(HWND /*hWnd*/, WPARAM /*wParam*/, LPARAM /*lParam*/){};
	virtual void				_onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/){};
	static int CALLBACK			s_pageWndProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);
	void						setChanged (bool bChanged); // Unables or disables apply button
	
	
private:
		
	PROPSHEETPAGE				m_page;
	HPROPSHEETPAGE	 			m_hdle;	
	HWND						m_hWnd;
	void*						m_pParent;	
	DLGPROC						m_pfnDlgProc;
		
};

