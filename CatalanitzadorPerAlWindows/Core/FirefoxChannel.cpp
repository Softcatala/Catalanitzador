/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "stdafx.h"
#include "FirefoxChannel.h"

#define DEFAULT_CHANNEL L"release"

FirefoxChannel::FirefoxChannel(wstring path) : FirefoxPreferencesFile(wstring())
{
	m_path = path;
}

wstring FirefoxChannel::GetChannel()
{
	if (m_channel.empty())
	{		
		wstring location;

		// Read from C:\Program Files\Mozilla Firefox\defaults\pref
		location = m_path;
		location += L"\\defaults\\pref\\channel-prefs.js";
		_readValue(location, L"app.update.channel", m_channel);

		// Default channel
		if (m_channel.empty())
		{
			m_channel = DEFAULT_CHANNEL;
		}
	}
	g_log.Log(L"FirefoxChannel::GetChannel returns %s", (wchar_t *) m_channel.c_str());
	return m_channel;
}				  
