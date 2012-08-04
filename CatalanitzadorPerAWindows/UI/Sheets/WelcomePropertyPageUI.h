﻿/* 
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
#include "ConfigurationDownload.h"
#include "Action.h"

#include <vector>

using namespace std;

class WelcomePropertyPageUI: public PropertyPageUI
{
public:
		WelcomePropertyPageUI();
		~WelcomePropertyPageUI();

		void SetSendStats(BOOL *pbSendStats) { m_pbSendStats = pbSendStats;}
		void SetSystemRestore(BOOL *pbSystemRestore) { m_pbSystemRestore = pbSystemRestore;}
		void SetActions(vector <Action *> * value) { m_actions = value;}
		virtual NotificationResult _onNotify(LPNMHDR /*hdr*/, int /*iCtrlID*/);		

private:
		virtual void _onInitDialog();
		virtual	bool _onNext();
		void _setTransparentBitmaps();
		void _initPropertySheet();
		Action* _getCatalanitzadorAction() const;		
		void _updateCatalanitzadorAction(Action* action);
		bool _doesUserWantToUpdate();

		HFONT		m_hFont;
		BOOL*		m_pbSendStats;
		BOOL*		m_pbSystemRestore;
		vector <Action *> * m_actions;
		ConfigurationDownload m_configurationDownload;
};
