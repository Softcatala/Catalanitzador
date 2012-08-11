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

#include "ActionID.h"
#include "ApplicationVersion.h"
#include "ConfigurationFileActionDownload.h"
#include "ApplicationVersion.h"
#include <string>
#include <vector>

using namespace std;

class ConfigurationFileActionDownloads
{
	public:
			ConfigurationFileActionDownloads() {}

			ActionID GetActionID() {return m_actionID;}
			void SetActionID(ActionID actionID) {m_actionID = actionID;}
	
			vector <ConfigurationFileActionDownload>& GetFileActionDownloadCollection() {return m_fileActionsDownload;}
			ConfigurationFileActionDownload& GetFileDownloadForVersion(ApplicationVersion version);
			ConfigurationFileActionDownload& GetDownloadForActionID(ActionID actionID, ApplicationVersion version);

			int AddFileActionDownload(ConfigurationFileActionDownload fileDownload)
			{
				m_fileActionsDownload.push_back(fileDownload);
				return m_fileActionsDownload.size() - 1;
			}


	private:

			ActionID m_actionID;
			vector <ConfigurationFileActionDownload> m_fileActionsDownload;
			ConfigurationFileActionDownload m_fileActionsDownloadEmpty;
};
