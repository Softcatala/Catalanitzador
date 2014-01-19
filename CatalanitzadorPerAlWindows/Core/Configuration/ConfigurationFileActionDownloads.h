/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include "ActionID.h"
#include "ActionStatus.h"
#include "ApplicationVersion.h"
#include "ConfigurationFileActionDownload.h"
#include "ApplicationVersion.h"
#include <string>
#include <vector>

using namespace std;

// TODO: This should be renamed to ConfigurationAction
class ConfigurationFileActionDownloads
{
	public:
			ConfigurationFileActionDownloads() : m_actionStatusHasValue(false) {}

			ActionID GetActionID() {return m_actionID;}
			void SetActionID(ActionID actionID) {m_actionID = actionID;}

			ActionStatus GetActionDefaultStatus() {return m_actionStatus;}
			bool GetActionDefaultStatusHasValue() {return m_actionStatusHasValue;}
			void SetActionDefaultStatus(ActionStatus actionStatus) 
			{
				m_actionStatus = actionStatus;
				m_actionStatusHasValue = true;
			}
	
			vector <ConfigurationFileActionDownload>& GetFileActionDownloadCollection() {return m_fileActionsDownload;}
			ConfigurationFileActionDownload& GetFileDownloadForVersion(ApplicationVersion version);
			ConfigurationFileActionDownload& GetFileDownloadForVersion(wstring version);

			int AddFileActionDownload(ConfigurationFileActionDownload fileDownload)
			{
				m_fileActionsDownload.push_back(fileDownload);
				return m_fileActionsDownload.size() - 1;
			}

	private:

			ActionID m_actionID;
			ActionStatus m_actionStatus;
			bool m_actionStatusHasValue;
			vector <ConfigurationFileActionDownload> m_fileActionsDownload;
			ConfigurationFileActionDownload m_fileActionsDownloadEmpty;
};
