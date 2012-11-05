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

#include "Thread.h"
#include "ConfigurationRemote.h"
#include <string>

using namespace std;

class ConfigurationDownloadThread : public Thread
{
	public:
			
			~ConfigurationDownloadThread();

			virtual void OnStart();

	private:

			wstring _getApplicationEmbeddedConfigurationSha1();
			bool _isEmmbeddedSha1EqualToRemote(wstring sha1_url, wstring sha1_file);
			bool _isConfigurationCompatibleWithAppVersion(ConfigurationRemote configuration);
			bool _getFile(wstring surl);
			bool _downloadRemoteSha1();
			void _setFileName();

			wstring m_filename;
			wstring m_donwloadSha1sum;
};
