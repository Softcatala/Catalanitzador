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

#include "PropertyPageUI.h"
#include "Action.h"
#include "CheckedListView.h"
#include <vector>
#include <map>
using namespace std;

class ApplicationsPropertyPageUI: public PropertyPageUI
{
public:
		~ApplicationsPropertyPageUI();
		ApplicationsPropertyPageUI();
		void SetActions(vector <Action *> * value) { m_availableActions =  value;}
		
private:

		void _updateActionDescriptionAndReq(Action* action);
		virtual void _onInitDialog();
		virtual NotificationResult _onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/);
		virtual	bool _onNext();
		void _noInternetConnection();
		static LRESULT _listViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void _processClickOnItem(int nItem);
		void _setBoldControls();
		void _setLegendControl();
		void _processDependantItem(Action* action);

		CheckedListView m_listview;
		HWND m_hList;
		HIMAGELIST m_hImageList;
		vector <Action *> * m_availableActions;
		typedef pair <Action *, bool> ActionBool_Pair;
		map <Action *, bool> m_disabledActions;
		WNDPROC PreviousProc;
		HFONT m_hFont;
};
