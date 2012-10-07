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
		ApplicationsPropertyPageUI();
		~ApplicationsPropertyPageUI();
		void SetActions(vector <Action *> * value) { m_availableActions =  value;}
		void SetDialectVariant(bool *dialectalVariant) { m_pbDialectalVariant = dialectalVariant;}
		void ProcessClickOnItem(int nItem);

private:
	
		void _updateActionDescriptionAndReq(Action* action);
		virtual void _onInitDialog();
		virtual NotificationResult _onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/);
		virtual void _onCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
		virtual	bool _onNext();
		void _showNoInternetConnectionDialog();
		void _setBoldControls();
		void _setLegendControl();
		void _processDependantItem(Action* action);
		bool _licenseAccepted();
		bool _licensesNeedToBeAccepted();
		void _enableOrDisableLicenseControls();
		void _getActionDisplayName(Action *action, wstring& name);
		bool _checkRunningApps();
		void _insertActioninListView(Action *action);
		void _insertGroupNameListView(ActionGroup group);
		bool _anyActionNeedsInternetConnection();
		static void _onClickItemEvent(int nItem, void* data);

		CheckedListView m_listview;
		CheckedListView m_listviewLegend;
		vector <Action *> * m_availableActions;
		typedef pair <Action *, bool> ActionBool_Pair;
		map <Action *, bool> m_disabledActions;		
		HFONT m_hFont;
		bool* m_pbDialectalVariant;
};
