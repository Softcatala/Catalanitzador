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
/*#include "ConfigurationLatest.h"

using ::testing::StrCaseEq;

#define LATEST_VERSION L"1.2.3"

TEST(ConfigurationLatestTest, GetSetVersion)
{	
	ConfigurationLatest configurationLatest;
	configurationLatest.SetVersion(LATEST_VERSION);
	
	EXPECT_THAT(configurationLatest.GetVersion(), StrCaseEq(LATEST_VERSION));
}

#define LATEST_URL1 L"http://url1.com"
#define LATEST_URL2 L"http://url1.com"

TEST(ConfigurationLatestTest, GetSetURL)
{	
	ConfigurationLatest configurationLatest;
	configurationLatest.AddUrl(LATEST_URL1);
	configurationLatest.AddUrl(LATEST_URL2);
	
	EXPECT_THAT(configurationLatest.GetUrls().size(), 2);
	EXPECT_THAT(configurationLatest.GetUrls()[0], StrCaseEq(LATEST_URL1));
	EXPECT_THAT(configurationLatest.GetUrls()[1], StrCaseEq(LATEST_URL2));
}

TEST(ConfigurationLatestTest, IsRunningInstanceUpToDate)
{	
	ConfigurationLatest configurationLatest;
	configurationLatest.AddUrl(LATEST_URL1);
	configurationLatest.AddUrl(LATEST_URL2);

	configurationLatest.SetVersion(L"10000.0.0");
	EXPECT_FALSE(configurationLatest.IsRunningInstanceUpToDate());

	configurationLatest.SetVersion(L"0.1.0");
	EXPECT_TRUE(configurationLatest.IsRunningInstanceUpToDate());
}

*/