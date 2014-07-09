/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "IRunner.h"
#include "IRegistry.h"
#include "OutLookHotmailConnector.h"
#include "MultipleDownloads.h"
#include "IOSVersion.h"
#include "IWin32I18N.h"

enum MSOfficeVersion
{
	MSOfficeUnKnown,
	NoMSOffice,
	MSOffice2003,
	MSOffice2007,
	MSOffice2010,
	MSOffice2010_64,
	MSOffice2013,
	MSOffice2013_64
};

class MSOffice
{
public:
		MSOffice(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, MSOfficeVersion version);
		~MSOffice();

		bool GetUseDialectalVariant() { return m_dialectalVariant; }
		void SetUseDialectalVariant(bool dialectalVariant) {m_dialectalVariant = dialectalVariant;}
		MSOfficeVersion GetVersionEnum() { return m_MSVersion; }
		void Execute();		
		void SetDefaultLanguage();
		bool IsLangPackInstalled();
		bool IsDefaultLanguage();
		const wchar_t* GetVersion();
		void AddDownloads(MultipleDownloads& multipleDownloads);
		void DumpLogOnError();
		bool DoesRequiereInstallLangPack() { return m_installedLangPackCode.size() == 0; }

protected:

		wchar_t* _getDownloadID();
		wstring m_packageCodeToSet;
		wstring m_installedLangPackCode;		
		
private:
		
		struct RegKeyVersion
		{
			wchar_t* VersionNumber;
			wchar_t* InstalledLangMapKey;
			bool InstalledLangMapKeyIsDWord;
		};

		RegKeyVersion _getRegKeys();

		static RegKeyVersion RegKeys2003;
		static RegKeyVersion RegKeys2007;
		static RegKeyVersion RegKeys2010;
		static RegKeyVersion RegKeys2013;

		ActionID _getID() const { return MSOfficeLPIActionID;};
		bool _extractCabFile(wchar_t * file, wchar_t * path);
		void _removeOffice2003TempFiles();
		void _readDefaultLanguage(bool& isCatalanSetAsDefaultLanguage, bool& followSystemUIOff);
		
		IRunner* m_runner;
		IRegistry* m_registry;
		IOSVersion* m_OSVersion;
		IWin32I18N* m_win32I18N;
		bool m_dialectalVariant;
		MSOfficeVersion m_MSVersion;
		wstring m_msiexecLog;

		wchar_t m_szFullFilename[MAX_PATH];
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szTempPath[MAX_PATH];
		wchar_t m_szTempPath2003[MAX_PATH];
};


