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
#include "OpenOfficeTest.h"

using ::testing::StrCaseEq;

#define LibreOffice_REGKEY L"SOFTWARE\\LibreOffice\\LibreOffice"

class LibreOfficeTest : public LibreOffice
{
public:	
	
	LibreOfficeTest::LibreOfficeTest(IRegistry* registry)
		: LibreOffice(registry) {};
	
	public:

		virtual wstring _getAppDataDir() {return L"\\directory"; }
		using LibreOffice::_getPreferencesDirectory;
};


#define CreateLibreOffice \
	RegistryMock registryMockobj; \
	LibreOfficeTest OpenOffice(&registryMockobj);

TEST(LibreOfficeTest, _getPreferencesFile_Version34)
{
	const wchar_t* LOO_VERSION = L"3.4";
	CreateLibreOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, LibreOffice_REGKEY, LOO_VERSION);
	wstring path = OpenOffice._getPreferencesDirectory();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\LibreOffice\\3\\user\\"));
}

TEST(LibreOfficeTest, _getPreferencesFile_Version40)
{
	const wchar_t* LOO_VERSION = L"4.0";
	CreateLibreOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, LibreOffice_REGKEY, LOO_VERSION);
	wstring path = OpenOffice._getPreferencesDirectory();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\LibreOffice\\4\\user\\"));
}

TEST(LibreOfficeTest, _getPreferencesFile_Version50)
{
	const wchar_t* LOO_VERSION = L"5.0";
	CreateLibreOffice;
	
	SetOpenOfficeAppVersion(registryMockobj, LibreOffice_REGKEY, LOO_VERSION);
	wstring path = OpenOffice._getPreferencesDirectory();
	EXPECT_THAT(path.c_str(), StrCaseEq(L"\\directory\\LibreOffice\\4\\user\\"));
}
