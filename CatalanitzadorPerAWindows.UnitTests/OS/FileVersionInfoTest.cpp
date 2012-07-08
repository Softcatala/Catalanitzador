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

#include "stdafx.h"
#include "Defines.h"
#include "FileVersionInfo.h"
#include "StringConversion.h"
#include "Version.h"

using ::testing::StrCaseEq;

#define UNITTESTS_BINARY L"CatalanitzadorPerAlWindows.UnitTests.exe"
#define LCID_CATALAN 1027

TEST(FileVersionInfoTest, _readVersion_testAgainstThisBinary)
{
	wstring version, binary;

	StringConversion::ToWideChar(STRING_VERSION, binary);
	FileVersionInfo fileVersionInfo(UNITTESTS_BINARY);
	version = fileVersionInfo.GetVersion();

	EXPECT_THAT(version.c_str(), StrCaseEq(binary.c_str()));
	EXPECT_THAT(fileVersionInfo.GetMajorVersion(), APP_MAJOR_VERSION);
}

TEST(FileVersionInfoTest, _readLanguageCode_testAgainstThisBinary)
{
	DWORD lang;
	
	FileVersionInfo fileVersionInfo(UNITTESTS_BINARY);
	lang = fileVersionInfo.GetLanguageCode();
	
	EXPECT_THAT(lang, LCID_CATALAN);
}
