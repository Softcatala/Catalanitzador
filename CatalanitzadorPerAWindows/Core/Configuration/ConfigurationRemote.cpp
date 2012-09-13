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
 
#include <stdafx.h>
#include "ConfigurationRemote.h"

static ConfigurationFileActionDownload s_empty;

ConfigurationRemote::ConfigurationRemote()
{
	_LARGE_INTEGER li = {0};
	SetDateTime(li);
}
	
int ConfigurationRemote::AddFileActionDownloads(ConfigurationFileActionDownloads fileDownloads)
{
	m_fileActionsDownloads.push_back(fileDownloads);
	return m_fileActionsDownloads.size() - 1;
}
			
ConfigurationFileActionDownload& ConfigurationRemote::GetDownloadForActionID(ActionID actionID, ApplicationVersion version)
{
	for (unsigned int i = 0; i < m_fileActionsDownloads.size(); i++)
	{
		if (m_fileActionsDownloads.at(i).GetActionID() == actionID)
		{
			return m_fileActionsDownloads.at(i).GetFileDownloadForVersion(version);
		}
	}	
	return s_empty;
}

ConfigurationFileActionDownload& ConfigurationRemote::GetDownloadForActionID(ActionID actionID, wstring version)
{
	for (unsigned int i = 0; i < m_fileActionsDownloads.size(); i++)
	{
		if (m_fileActionsDownloads.at(i).GetActionID() == actionID)
		{
			return m_fileActionsDownloads.at(i).GetFileDownloadForVersion(version);						
		}
	}	
	return s_empty;
}

bool ConfigurationRemote::IsOlderThan(ConfigurationRemote other)
{
	return GetDateTime().QuadPart < other.GetDateTime().QuadPart;
}

