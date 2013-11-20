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
#include "Defines.h"
#include "RegistryMock.h"
#include "LibreOffice.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;
using ::testing::HasSubstr;

#define ENUM_REG_INDEX0 0
#define ENUM_REG_INDEX1 1

class LibreOfficeActionTest : public LibreOffice
{
public:	
	
	LibreOfficeActionTest::LibreOfficeActionTest(IRegistry* registry)
		: LibreOffice(registry) {};
	
	public:

		virtual wstring _getAppDataDir() {return L"\\directory"; }

		using LibreOffice::_readLibreOfficeVersionInstalled;
		using LibreOffice::_readLibreOfficeInstallPath;
		using LibreOffice::_getPreferencesFile;		
};


#define CreateLibreOffice \
	RegistryMock registryMockobj; \
	LibreOfficeActionTest libreOffice(&registryMockobj);

void SetLibreOfficeVersion(RegistryMock& registryMockobj, wstring version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(LIBREOFFICE_REGKEY), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(version), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));
}

void SetLibreOfficeInstallPath(RegistryMock& registryMockobj, wstring version, const wchar_t* path)
{
	wstring key = L"SOFTWARE\\LibreOffice\\LibreOffice\\" + version;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(key.c_str()), false)).
		WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(path), Return(true)));
}

TEST(LibreOfficeTest, _readLibreOfficeVersionInstalled)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.3";
	CreateLibreOffice;
	
	SetLibreOfficeVersion(registryMockobj, OPENOFFICE_VERSION);	
	libreOffice._readLibreOfficeVersionInstalled();
	EXPECT_THAT(libreOffice.GetVersion(), StrCaseEq(OPENOFFICE_VERSION));
}

TEST(LibreOfficeTest, _readLibreOfficeInstallPath)
{
	const wchar_t* OPENOFFICE_VERSION = L"4.1";
	CreateLibreOffice;
	
	SetLibreOfficeVersion(registryMockobj, OPENOFFICE_VERSION);	
	SetLibreOfficeInstallPath(registryMockobj, OPENOFFICE_VERSION, L"\\somepath\\bin.exe");

	libreOffice._readLibreOfficeInstallPath();
	wstring path = libreOffice.GetInstallationPath();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\somepath\\"));
}

TEST(LibreOfficeTest, _getPreferencesFile_Version4)
{
	const wchar_t* OPENOFFICE_VERSION = L"4.1";
	CreateLibreOffice;
	
	SetLibreOfficeVersion(registryMockobj, OPENOFFICE_VERSION);
	wstring path = libreOffice._getPreferencesFile();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\LibreOffice\\4\\user\\"));
}

TEST(LibreOfficeTest, _getPreferencesFile_Version34)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.4";
	CreateLibreOffice;
	
	SetLibreOfficeVersion(registryMockobj, OPENOFFICE_VERSION);
	wstring path = libreOffice._getPreferencesFile();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\LibreOffice\\3\\user\\"));
}
