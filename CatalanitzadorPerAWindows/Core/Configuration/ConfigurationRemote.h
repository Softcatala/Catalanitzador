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

#include "ConfigurationFileActionDownloads.h"
#include "ActionID.h"
#include <string>

using namespace std;

static ConfigurationFileActionDownload s_empty;

class ConfigurationRemote
{
	public:
	
			wstring& GetCompatibility() {return m_compatibility;}
			void SetCompatibility(wstring compatibility) {m_compatibility = compatibility;}

			vector <ConfigurationFileActionDownloads>& GetFileActionsDownloads() {return m_fileActionsDownloads;}

			int AddFileActionDownloads(ConfigurationFileActionDownloads fileDownloads)
			{
				m_fileActionsDownloads.push_back(fileDownloads);
				return m_fileActionsDownloads.size() - 1;
			}
			
			ConfigurationFileActionDownload& GetDownloadForActionID(ActionID actionID, ApplicationVersion version)
			{
				for (unsigned i = 0; i < m_fileActionsDownloads.size(); i++)
				{
					if (m_fileActionsDownloads.at(i).GetActionID() == actionID)
					{
						return m_fileActionsDownloads.at(i).GetFileDownloadForVersion(version);
					}
				}	
				return s_empty;
			}

	private:
			
			wstring m_compatibility;
			vector <ConfigurationFileActionDownloads> m_fileActionsDownloads;	
};
