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
#include "OpenOffice.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define OPENOFFICE_REGKEY L"SOFTWARE\\OpenOffice.org\\OpenOffice.org"
#define ENUM_REG_INDEX0 0
#define ENUM_REG_INDEX1 1

class OpenOfficeTest : public OpenOffice
{
public:	
	
	OpenOfficeTest::OpenOfficeTest(IRegistry* registry)
		: OpenOffice(registry) {};
	
	public:

		virtual wstring _getAppDataDir() {return L"\\directory"; }

		using OpenOffice::_readVersionInstalled;
		using OpenOffice::_readInstallPath;
		using OpenOffice::_getPreferencesFile;		
};


#define CreateOpenOffice \
	RegistryMock registryMockobj; \
	OpenOfficeTest OpenOffice(&registryMockobj);

void SetOpenOfficeAppVersion(RegistryMock& registryMockobj, wstring key, wstring version)
{
	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(key.c_str()), false)).WillRepeatedly(Return(true));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX0,_)).WillRepeatedly(DoAll(SetArgWStringPar2(version), Return(true)));
	EXPECT_CALL(registryMockobj, RegEnumKey(ENUM_REG_INDEX1,_)).WillRepeatedly(Return(false));
}

void SetOpenOfficeInstallPath(RegistryMock& registryMockobj, wstring version, const wchar_t* path)
{
	wstring key = L"SOFTWARE\\OpenOffice.org\\OpenOffice.org\\" + version;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(key.c_str()), false)).
		WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(path), Return(true)));
}

TEST(OpenOfficeTest, _readOpenOfficeVersionInstalled)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.3";
	CreateOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, OPENOFFICE_REGKEY, OPENOFFICE_VERSION);	
	OpenOffice._readVersionInstalled();
	EXPECT_THAT(OpenOffice.GetVersion(), StrCaseEq(OPENOFFICE_VERSION));
}

TEST(OpenOfficeTest, _readOpenOfficeInstallPath)
{
	const wchar_t* OPENOFFICE_VERSION = L"4.1";
	CreateOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, OPENOFFICE_REGKEY, OPENOFFICE_VERSION);	
	SetOpenOfficeInstallPath(registryMockobj, OPENOFFICE_VERSION, L"\\somepath\\bin.exe");

	OpenOffice._readInstallPath();
	wstring path = OpenOffice.GetInstallationPath();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\somepath\\"));
}

TEST(OpenOfficeTest, _getPreferencesFile_Version4)
{
	const wchar_t* OPENOFFICE_VERSION = L"4.1";
	CreateOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, OPENOFFICE_REGKEY, OPENOFFICE_VERSION);
	wstring path = OpenOffice._getPreferencesFile();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\OpenOffice.org\\4\\user\\"));
}

TEST(OpenOfficeTest, _getPreferencesFile_Version34)
{
	const wchar_t* OPENOFFICE_VERSION = L"3.4";
	CreateOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, OPENOFFICE_REGKEY, OPENOFFICE_VERSION);
	wstring path = OpenOffice._getPreferencesFile();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\OpenOffice.org\\3\\user\\"));
}
