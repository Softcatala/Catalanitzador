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
#include "ApacheOpenOffice.h"
#include "OpenOfficeTest.h"

using ::testing::StrCaseEq;
using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;

#define APACHEOPENOFFICE_REGKEY L"SOFTWARE\\OpenOffice\\OpenOffice"

class ApacheOpenOfficeTest : public ApacheOpenOffice
{
public:

	ApacheOpenOfficeTest::ApacheOpenOfficeTest(IRegistry* registry)
		: ApacheOpenOffice(registry) {};

	virtual wstring _getAppDataDir() {return L"\\directory"; }
	using ApacheOpenOffice::_getPreferencesDirectory;
	using ApacheOpenOffice::_getInstallationPath;
};


#define CreateApacheOpenOffice \
	RegistryMock registryMockobj; \
	ApacheOpenOfficeTest apacheOpenOffice(&registryMockobj);

void SetApacheOpenOfficeInstallPath(RegistryMock& registryMockobj, wstring version, const wchar_t* path)
{
	wstring key = L"SOFTWARE\\OpenOffice\\OpenOffice\\" + version;

	EXPECT_CALL(registryMockobj, OpenKey(HKEY_LOCAL_MACHINE, StrCaseEq(key.c_str()), false)).
		WillRepeatedly(Return(true));

	EXPECT_CALL(registryMockobj, GetString(StrCaseEq(L"Path"),_ ,_)).
		WillRepeatedly(DoAll(SetArgCharStringPar2(path), Return(true)));
}

TEST(ApacheOpenOfficeTest, _getPreferencesFile_Version34)
{
	const wchar_t* APACHE_VERSION = L"3.4";
	CreateApacheOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, APACHEOPENOFFICE_REGKEY, APACHE_VERSION, false);
	wstring path = apacheOpenOffice._getPreferencesDirectory();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\OpenOffice\\3\\user\\"));
}

TEST(ApacheOpenOfficeTest, _getInstallationPath)
{
	const wchar_t* APACHEOPENOFFICE_VERSION = L"4.1";
	CreateApacheOpenOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, APACHEOPENOFFICE_REGKEY, APACHEOPENOFFICE_VERSION, false);
	SetApacheOpenOfficeInstallPath(registryMockobj, APACHEOPENOFFICE_VERSION, L"\\somepath\\bin.exe");
	
	wstring path = apacheOpenOffice._getInstallationPath();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\somepath\\"));
}
