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
#include "CommandLine.h"
#include "ConfigurationInstance.h"

using ::testing::StrCaseEq;

#define FORCED_VERSION L"4.3.2"

TEST(CommandLineTest, _Version)
{
	wstring version(L"/version:");

	version += FORCED_VERSION;
	CommandLine commandLine(NULL);
	commandLine.Process(version);

	EXPECT_THAT(ConfigurationInstance::Get().GetVersion().GetString(), StrCaseEq(FORCED_VERSION));
}

TEST(CommandLineTest, _UseAeroLook)
{	
	wstring parameters(L"/UseAeroLook");
	
	CommandLine commandLine(NULL);
	commandLine.Process(parameters);
	EXPECT_TRUE(ConfigurationInstance::Get().GetAeroEnabled());
}

TEST(CommandLineTest, _UseClassicLook)
{	
	wstring parameters(L"/UseClassicLook");
	
	CommandLine commandLine(NULL);
	commandLine.Process(parameters);
	EXPECT_FALSE(ConfigurationInstance::Get().GetAeroEnabled());
}

TEST(CommandLineTest, _RunningCheck)
{	
	wstring parameters(L"/norunningcheck");	
	CommandLine commandLine(NULL);
	
	commandLine.Process(parameters);
	EXPECT_FALSE(commandLine.GetRunningCheck());
}

TEST(CommandLineTest, _RunningCheckWithVersion)
{	
	const wchar_t* VERSION = L"0.1.3";
	wstring version_found;
	Actions acts(NULL);
	vector <Action *>  actions = acts.GetActions();

	wstring parameters(L"/norunningcheck:");
	CommandLine commandLine(&acts);

	parameters+= VERSION;

	EXPECT_TRUE(commandLine.GetRunningCheck());
	
	commandLine.Process(parameters);
	
	EXPECT_FALSE(commandLine.GetRunningCheck());
	
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


TEST(CommandLineTest, _NoConfigurationDownload)
{
	wstring parameters(L"/NoConfigurationDownload");
	CommandLine commandLine(NULL);
	
	commandLine.Process(parameters);
	EXPECT_FALSE(ConfigurationInstance::Get().GetDownloadConfiguration());
}

#define DEFINED_URL L"http://www.softcatala.org/pub/catalanitzador/configuration_test.xml"

TEST(CommandLineTest, _ConfigurationDownloadUrl)
{
	wstring parameters(L"/ConfigurationDownloadUrl:");
	CommandLine commandLine(NULL);

	parameters+=DEFINED_URL;	
	commandLine.Process(parameters);
	EXPECT_THAT(ConfigurationInstance::Get().GetDownloadConfigurationUrl(), StrCaseEq(DEFINED_URL));
}