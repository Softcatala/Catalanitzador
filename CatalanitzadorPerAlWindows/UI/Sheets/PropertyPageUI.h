/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

class PropertySheetUI;

enum PropertyPageButton
{
	DefaultButtons,
	NextButton,
	BackButton,
	NextBackButtons,
	FinishButtonOnly,
	CancelButtonOnly,
};

enum NotificationResult
{
	ReturnTrue,
	ReturnFalse,
	CallDefProc
};

class PropertyPageUI
{
public:
	
	PropertyPageUI();
	~PropertyPageUI();
	
	void 						createPage(HINSTANCE hInstance, WORD wRscIDAero, LPWSTR pTitle);	
	PROPSHEETPAGE*				getStruct(){return &m_page;}
	HWND						getHandle(){return m_hWnd;}
	PropertySheetUI*			getParent(){return m_sheet;}
	void						setParent(PropertySheetUI* sheet){m_sheet = sheet; }
	void						setDialogProc(DLGPROC pfnDlgProc){m_pfnDlgProc=pfnDlgProc;};		
	void						setPageButtons (PropertyPageButton buttons) { m_PageButtons = buttons; }	
	virtual	bool				_onCancel();

private:

	static int CALLBACK			s_pageWndProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);

	virtual	void				_onInitDialog(){};
	virtual	void				_sendSetButtonsMessage();
	virtual	void				_onKillActive(){};
	virtual	void				_onOK(){};
	virtual	void				_onApply(){};
	virtual	bool				_onNext(){return true;};
	virtual	void				_onFinish(){};
	virtual	void				_onShowWindow(){};
	virtual	void				_onTimer(){};
	virtual	void				_onEndSession(){};
	virtual void				_onCommand(HWND /*hWnd*/, WPARAM /*wParam*/, LPARAM /*lParam*/){};
	virtual NotificationResult	_onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/){ return ReturnFalse;};
	
		
	PROPSHEETPAGE				m_page;	
	HWND						m_hWnd;
	PropertySheetUI*			m_sheet;
	DLGPROC						m_pfnDlgProc;
	PropertyPageButton			m_PageButtons;
};

