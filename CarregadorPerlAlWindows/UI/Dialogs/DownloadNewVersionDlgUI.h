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

#include "DlgUI.h"
#include "Action.h"

#include <string>
using namespace std;


class DownloadNewVersionDlgUI : public DlgUI
{
public:
		DownloadNewVersionDlgUI(Action* pUpdateAction) {m_pUpdateAction = pUpdateAction;}

		virtual LPCTSTR GetResourceTemplate() {return MAKEINTRESOURCE(IDD_DOWNLOADNEWVERSION);}

		virtual	void _onInitDialog();
		virtual	void _onTimer();
		virtual void _onCommand(WPARAM wParam, LPARAM lParam);
		bool OnDownloadStatus(int total, int current);

private:		
		static bool _downloadStatus(int total, int current, void *data);

		Action* m_pUpdateAction;
		HWND m_hProgressBar;
		HWND m_hDescription;		
		bool m_bCancelled;
};
