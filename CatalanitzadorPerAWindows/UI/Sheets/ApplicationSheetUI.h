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

#include "PropertySheetUI.h"
#include "DownloadManager.h"

#include "Action.h"
#include <vector>
using namespace std;

class ApplicationSheetUI: public PropertySheetUI
{
public:
		ApplicationSheetUI();
		~ApplicationSheetUI();

		virtual	void _onInitDialog();

		void SetActions(vector <Action *> * value) { m_actions =  value;}
		vector <Action *> * GetActions() { return m_actions;}
		DownloadManager GetDownloadManager() { return m_downloadManager;}
		void SetDownloadManager(DownloadManager downloadManager) { m_downloadManager = downloadManager;}		

private:
		void _disableCloseButton();

		HICON m_hIcon;
		vector <Action *> * m_actions;
		DownloadManager m_downloadManager;
};
