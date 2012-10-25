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
#include "Configuration.h"

using ::testing::StrCaseEq;
using ::testing::Return;

#define COMPATIBILITY L"3.2.1"

TEST(ConfigurationTest, GetSetRemote)
{
	Configuration configuration(new OSVersion());
	ConfigurationRemote configurationRemote;
	configurationRemote.SetCompatibility(COMPATIBILITY);	
	configuration.SetRemote(configurationRemote);
	
	EXPECT_THAT(configuration.GetRemote().GetCompatibility(), StrCaseEq(COMPATIBILITY));
}

TEST(ConfigurationTest, GetAeroAuto_WindowsXP)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));

	Configuration configuration(&osVersionExMock);
	EXPECT_FALSE(configuration.GetAeroEnabled());
}

TEST(ConfigurationTest, GetAeroAuto_Windows7)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));

	Configuration configuration(&osVersionExMock);
	EXPECT_TRUE(configuration.GetAeroEnabled());
}

TEST(ConfigurationTest, GetAeroGetSet_Windows7)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));

	Configuration configuration(&osVersionExMock);
	configuration.SetAeroEnabled(false);
	EXPECT_FALSE(configuration.GetAeroEnabled());
}