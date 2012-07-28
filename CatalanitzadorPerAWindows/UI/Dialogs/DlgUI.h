/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

class DlgUI
{
public:
		int Run(HWND hWnd);
		virtual LPCTSTR GetResourceTemplate() = 0;		

protected:

		virtual	void _onInitDialog(){};
		virtual void _onCommand(WPARAM wParam, LPARAM lParam){};
		virtual int _onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/){ return 0;}
		virtual	void _onTimer() {};
		HWND getHandle() { return  m_hWnd;}

private:
		HWND m_hWnd;
		static LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);		
};
