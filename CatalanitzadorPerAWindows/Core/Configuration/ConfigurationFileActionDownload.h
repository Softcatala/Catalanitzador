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
#include <string>
#include <assert.h>

using namespace std;

#define SHA1_EXTENSION L".sha1"

class _APICALL ConfigurationFileActionDownload
{
	public:
			ConfigurationFileActionDownload() {}

			bool IsEmpty()
			{
				return m_urls.size() == 0 &&
					m_maxVersion.GetString().empty() &&
					m_minVersion.GetString().empty();
			}

			ApplicationVersion& GetMaxVersion() {return m_maxVersion;}
			void SetMaxVersion(ApplicationVersion version) {m_maxVersion = version;}

			ApplicationVersion& GetMinVersion() {return m_minVersion;}
			void SetMinVersion(ApplicationVersion version) {m_minVersion = version;}

			wstring& GetFilename() {return m_filename;}
			void SetFilename(wstring filename) {m_filename = filename;}

			wstring& GetVersion() {return m_version;}
			void SetVersion(wstring version) {m_version = version;}
	
			vector <wstring>& GetUrls() {return m_urls;}
			void AddUrl(wstring url)
			{
				m_urls.push_back(url);
				m_sha1_urls.push_back(url + SHA1_EXTENSION);
			}

			vector <wstring>& GetSha1Urls() {return m_sha1_urls;}
			void SetSha1Url(unsigned int index, wstring url)
			{
				assert(index <= m_sha1_urls.size());
				m_sha1_urls[index] = url;
			}

	private:

			ApplicationVersion m_maxVersion;
			ApplicationVersion m_minVersion;
			wstring m_filename;
			wstring m_version;
			vector <wstring> m_urls;
			vector <wstring> m_sha1_urls;
};


