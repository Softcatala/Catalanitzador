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

#pragma once

#include "DownloadManager.h"

using namespace std;

class FirefoxMozillaServer
{
public:
		FirefoxMozillaServer(DownloadManager* downloadManager, wstring version);
		ConfigurationFileActionDownload GetConfigurationFileActionDownload();
		wstring GetSha1FileSignature(ConfigurationFileActionDownload downloadVersion);

protected:
		wstring _getSha1SignatureForFilename(wstring sha1file, wstring filename);
		void _replaceString(wstring& string, wstring search, wstring replace);

private:

		wstring _readSha1FileSignature(wstring url, wstring sha1file);

		wstring m_version;
		DownloadManager* m_downloadManager;
};
