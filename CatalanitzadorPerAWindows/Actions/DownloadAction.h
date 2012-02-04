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

#pragma warning(disable:4251)

#include <windows.h>
#include "DownloadInet.h"
#include "ProgressStatus.h"
#include "Serializable.h"
#include "ActionStatus.h"
#include "ActionID.h"
#include "Sha1Sum.h"
#include "DownloadActionData.h"

#include <vector>
using namespace std;

#define SHA1_EXTESION L".sha1"


class Download
{

public:

	Download() {}

	Download(wstring _download, int _id)
	{
		download = _download;
		id = _id;
	}

	wstring download;
	int id;	
};

class _APICALL DownloadAction
{
public:	
		DownloadAction();
		
		bool GetFile(DownloadID downloadID, wstring file, ProgressStatus progress, void *data);
		bool GetAssociatedFileSha1Sum(DownloadID downloadID, wstring sha1_file, Sha1Sum &sha1sum);
		wstring GetFileName(DownloadID downloadID);
		vector <Download> GetDownloads() {return m_downloads;}

private:
		void _getRebost(DownloadID downloadID, wstring &url);
		
		vector <Download> m_downloads;
};

