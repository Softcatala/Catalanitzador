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
#include "ApplicationVersion.h"

using ::testing::StrCaseEq;
using ::testing::StrCaseNe;


TEST(VersionTest, GetComponents_Three)
{
	ApplicationVersion version (L"1.2.3");
	vector <wstring> components;
	
	components = version.GetComponents();

	EXPECT_THAT(components.size(), 3);
	EXPECT_THAT(components[0], StrCaseEq(L"1"));
	EXPECT_THAT(components[1], StrCaseEq(L"2"));
	EXPECT_THAT(components[2], StrCaseEq(L"3"));
}

TEST(VersionTest, GetComponents_Two)
{
	ApplicationVersion version (L"1.2");
	vector <wstring> components;
	
	components = version.GetComponents();

	EXPECT_THAT(components.size(), 2);
	EXPECT_THAT(components[0], StrCaseEq(L"1"));
	EXPECT_THAT(components[1], StrCaseEq(L"2"));	
}

TEST(VersionTest, GetComponents_UnBreakable)
{
	ApplicationVersion version (L"123");
	vector <wstring> components;
	
	components = version.GetComponents();

	EXPECT_THAT(components.size(), 1);
	EXPECT_THAT(components[0], StrCaseEq(L"123"));
}

TEST(VersionTest, GetComponents_Equal)
{
	ApplicationVersion versionA (L"1.2.3");
	ApplicationVersion versionB (L"1.2.3");
	ApplicationVersion versionC (L"1.2.1");

	EXPECT_TRUE(versionA == versionB);
	EXPECT_FALSE(versionA == versionC);
}

TEST(VersionTest, GetComponents_NotEqual)
{
	ApplicationVersion versionA (L"1.2.3");
	ApplicationVersion versionB (L"1.2.3");
	ApplicationVersion versionC (L"1.2.1");

	EXPECT_FALSE(versionA != versionB);
	EXPECT_TRUE(versionA != versionC);
}

TEST(VersionTest, GetComponents_Greater)
{
	ApplicationVersion versionA (L"1.2.0");
	ApplicationVersion versionB (L"1.2.1");
	ApplicationVersion versionC (L"1.2.2");
	ApplicationVersion versionD (L"1.1.0");
	ApplicationVersion versionE (L"0.9.0");

	EXPECT_TRUE(versionB > versionA);
	EXPECT_TRUE(versionC > versionB);
	EXPECT_TRUE(versionB > versionD);
	EXPECT_TRUE(versionC > versionE);
	EXPECT_FALSE(versionE > versionA);
}

TEST(VersionTest, GetComponents_Less)
{
	ApplicationVersion versionA (L"1.2.0");
	ApplicationVersion versionB (L"1.2.1");
	ApplicationVersion versionC (L"1.2.2");
	ApplicationVersion versionD (L"1.1.0");
	ApplicationVersion versionE (L"0.9.0");

	EXPECT_TRUE(versionA < versionB);
	EXPECT_TRUE(versionB < versionC);
	EXPECT_TRUE(versionD < versionB);
	EXPECT_TRUE(versionE < versionC);
	EXPECT_FALSE(versionA < versionE);
	EXPECT_FALSE(versionA < versionA);
}

TEST(VersionTest, GetComponents_GreaterOrEqual)
{
	ApplicationVersion versionA (L"1.2.0");
	ApplicationVersion versionB (L"1.2.1");
	ApplicationVersion versionC (L"1.2.2");
	ApplicationVersion versionD (L"1.1.0");
	ApplicationVersion versionE (L"0.9.0");

	EXPECT_TRUE(versionB >= versionA);
	EXPECT_TRUE(versionC >= versionB);
	EXPECT_TRUE(versionB >= versionD);
	EXPECT_TRUE(versionC >= versionE);
	EXPECT_FALSE(versionE >= versionA);
	EXPECT_TRUE(versionA >= versionA);
}

TEST(VersionTest, GetComponents_LessOrEqual)
{
	ApplicationVersion versionA (L"1.2.0");
	ApplicationVersion versionB (L"1.2.1");
	ApplicationVersion versionC (L"1.2.2");
	ApplicationVersion versionD (L"1.1.0");
	ApplicationVersion versionE (L"0.9.0");

	EXPECT_TRUE(versionA <= versionB);
	EXPECT_TRUE(versionB <= versionC);
	EXPECT_TRUE(versionD <= versionB);
	EXPECT_TRUE(versionE <= versionC);
	EXPECT_FALSE(versionA <= versionE);
	EXPECT_TRUE(versionE <= versionE);
}