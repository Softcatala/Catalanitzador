﻿/* 
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
#include "CatalanitzadorPerAWindows.h"
#include "ConfigurationInstance.h"

using ::testing::StrCaseEq;

class CatalanitzadorPerAWindowsTest : public CatalanitzadorPerAWindows
{
public:
	
	CatalanitzadorPerAWindowsTest() 
		: CatalanitzadorPerAWindows(GetModuleHandle(NULL)) {}
	
	public:
			using CatalanitzadorPerAWindows::_processCommandLine;

			bool GetRunningCheck() {return m_bRunningCheck;}
			Actions& GetActions() {return m_actions; }

};

#define FORCED_VERSION L"4.3.2"

TEST(CatalanitzadorPerAWindowsTest, _Version)
{
	wstring version(L"/version:");

	version += FORCED_VERSION;
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;
	catalanitzadorPerAWindows._processCommandLine(version);

	EXPECT_THAT(ConfigurationInstance::Get().GetVersion().GetString(), StrCaseEq(FORCED_VERSION));
}

TEST(CatalanitzadorPerAWindowsTest, _UseAeroLook)
{
	wstring parameters(L"/UseAeroLook");
	
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;
	catalanitzadorPerAWindows._processCommandLine(parameters);
	EXPECT_TRUE(ConfigurationInstance::Get().GetAeroEnabled());
}

TEST(CatalanitzadorPerAWindowsTest, _UseClassicLook)
{
	wstring parameters(L"/UseClassicLook");
	
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;
	catalanitzadorPerAWindows._processCommandLine(parameters);
	EXPECT_FALSE(ConfigurationInstance::Get().GetAeroEnabled());
}

TEST(CatalanitzadorPerAWindowsTest, _RunningCheck)
{
	wstring parameters(L"/norunningcheck");	
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;

	EXPECT_TRUE(catalanitzadorPerAWindows.GetRunningCheck());
	catalanitzadorPerAWindows._processCommandLine(parameters);
	EXPECT_FALSE(catalanitzadorPerAWindows.GetRunningCheck());
}

TEST(CatalanitzadorPerAWindowsTest, _RunningCheckWithVersion)
{
	const wchar_t* VERSION = L"0.1.3";
	wstring version_found;

	wstring parameters(L"/norunningcheck:");
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;

	parameters+= VERSION;

	EXPECT_TRUE(catalanitzadorPerAWindows.GetRunningCheck());
	
	catalanitzadorPerAWindows._processCommandLine(parameters);
	
	EXPECT_FALSE(catalanitzadorPerAWindows.GetRunningCheck());

	vector <Action *>  actions = catalanitzadorPerAWindows.GetActions().GetActions();
	
	for (unsigned int i = 0; i < actions.size(); i++)
	{
		if (actions.at(i)->GetID() != CatalanitzadorUpdate)
			continue;

		version_found = actions.at(i)->GetVersion();
		break;
	}

	// By checking the version we also are checking the the action was added to the list
	EXPECT_THAT(version_found, StrCaseEq(VERSION));
}


TEST(CatalanitzadorPerAWindowsTest, _NoConfigurationDownload)
{
	wstring parameters(L"/NoConfigurationDownload");	
	CatalanitzadorPerAWindowsTest catalanitzadorPerAWindows;
	
	catalanitzadorPerAWindows._processCommandLine(parameters);
	EXPECT_FALSE(ConfigurationInstance::Get().GetDownloadConfiguration());
}

