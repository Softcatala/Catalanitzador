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
#include "Runner.h"

enum MSOfficeVersion
{
	MSOfficeUnKnown,
	NoMSOffice,
	MSOffice2003,
	MSOffice2007,
	MSOffice2010
};

struct RegKeyVersion
{
	wchar_t* VersionNumber;
	wchar_t* InstalledLangMapKey;
	bool InstalledLangMapKeyIsDWord;
};

class MSOfficeAction : public Action
{
public:
		MSOfficeAction();
		~MSOfficeAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual int GetID() { return MSOfficeLPI;};
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual char* GetVersion();

private:

		void _getVersionInstalled();
		bool _isVersionInstalled(RegKeyVersion regkeys);
		bool _isLangPackForVersionInstalled(RegKeyVersion regkeys);		
		bool _wasInstalledCorrectly();
		bool _extractCabFile(wchar_t * file, wchar_t * path);
		void _setDefaultLanguage();		
		DownloadID  _getDownloadID();
		void _removeOffice2003TempFiles();
		RegKeyVersion _getRegKeys();

		bool m_bLangPackInstalled;
		wchar_t m_szFullFilename[MAX_PATH];
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szTempPath[MAX_PATH];
		wchar_t m_szTempPath2003[MAX_PATH];
		Runner runner;		
		MSOfficeVersion m_MSVersion;		
};

