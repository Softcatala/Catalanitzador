/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Actions.h"
#include "WindowsLPIAction.h"
#include "Windows8LPIAction.h"
#include "ConfigureDefaultLanguageAction.h"

using ::testing::Return;


TEST(ActionsTest, _buildListOfActions_WIN_XP)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsXP));

	Actions actions(NULL, &osVersionExMock);	
	WindowsLPIAction* windowsAction = dynamic_cast<WindowsLPIAction*>(actions.GetActionFromID(WindowsLPIActionID));
	ConfigureDefaultLanguageAction* configureDefaultLanguageAction = dynamic_cast<ConfigureDefaultLanguageAction*>(actions.GetActionFromID(ConfigureDefaultLanguageActionID));
	
	EXPECT_TRUE(windowsAction != NULL);
	EXPECT_TRUE(configureDefaultLanguageAction != NULL);
}

TEST(ActionsTest, _buildListOfActions_WIN_VISTA)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(WindowsVista));

	Actions actions(NULL, &osVersionExMock);	
	WindowsLPIAction* windowsAction = dynamic_cast<WindowsLPIAction*>(actions.GetActionFromID(WindowsLPIActionID));
	ConfigureDefaultLanguageAction* configureDefaultLanguageAction = dynamic_cast<ConfigureDefaultLanguageAction*>(actions.GetActionFromID(ConfigureDefaultLanguageActionID));
	
	EXPECT_TRUE(windowsAction != NULL);
	EXPECT_TRUE(configureDefaultLanguageAction != NULL);
}

TEST(ActionsTest, _buildListOfActions_WIN_7)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows7));

	Actions actions(NULL, &osVersionExMock);
	WindowsLPIAction* windowsAction = dynamic_cast<WindowsLPIAction*>(actions.GetActionFromID(WindowsLPIActionID));
	ConfigureDefaultLanguageAction* configureDefaultLanguageAction = dynamic_cast<ConfigureDefaultLanguageAction*>(actions.GetActionFromID(ConfigureDefaultLanguageActionID));
	
	EXPECT_TRUE(windowsAction != NULL);
	EXPECT_TRUE(configureDefaultLanguageAction != NULL);
}

TEST(ActionsTest, _buildListOfActions_WIN_8)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows8));

	Actions actions(NULL, &osVersionExMock);
	Windows8LPIAction* windowsAction = dynamic_cast<Windows8LPIAction*>(actions.GetActionFromID(WindowsLPIActionID));
	ConfigureDefaultLanguageAction* configureDefaultLanguageAction = dynamic_cast<ConfigureDefaultLanguageAction*>(actions.GetActionFromID(ConfigureDefaultLanguageActionID));
	
	EXPECT_TRUE(windowsAction != NULL);
	EXPECT_TRUE(configureDefaultLanguageAction == NULL);
}

TEST(ActionsTest, _buildListOfActions_WIN_81)
{
	OSVersionMock osVersionExMock;
	EXPECT_CALL(osVersionExMock, GetVersion()).WillRepeatedly(Return(Windows81));

	Actions actions(NULL, &osVersionExMock);
	Windows8LPIAction* windowsAction = dynamic_cast<Windows8LPIAction*>(actions.GetActionFromID(WindowsLPIActionID));
	ConfigureDefaultLanguageAction* configureDefaultLanguageAction = dynamic_cast<ConfigureDefaultLanguageAction*>(actions.GetActionFromID(ConfigureDefaultLanguageActionID));
	
	EXPECT_TRUE(windowsAction != NULL);
	EXPECT_TRUE(configureDefaultLanguageAction == NULL);
}



