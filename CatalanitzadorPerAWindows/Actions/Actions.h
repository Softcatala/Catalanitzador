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

#include "Action.h"
#include "IOSVersion.h"
#include "IWin32I18N.h"
#include "IRegistry.h"
#include "IRunner.h"
#include "IFileVersionInfo.h"
#include "DownloadManager.h"

#include <vector>

using namespace std;

class Actions
{
    public:
		Actions(DownloadManager downloadManager);
		~Actions();
		vector <Action *> GetActions() {return m_actions; }
		Action* GetActionFromID(ActionID actionID);

	private:
		void _buildListOfActions();
		void _checkPrerequirements();

		IOSVersion* _getNewOSVersion();
		IRegistry* _getNewRegistry();
		IRunner* _getNewRunner();
		IWin32I18N* _getNewWin32I18N();
		IFileVersionInfo* _getFileVersionInfo();

		vector <Action *> m_actions;
		vector <void *> m_objectsToDelete;
		DownloadManager m_downloadManager;
};
