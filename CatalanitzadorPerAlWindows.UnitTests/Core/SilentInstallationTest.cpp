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
#include "SilentInstallation.h"
#include "ActionExecutionMock.h"
#include "ActionMock.h"

using ::testing::Return;
using ::testing::_;

class SilentInstallationAction : public ActionMock, public ActionExecutionMock {};

class SilentInstallationTest : public SilentInstallation
{
public:

	using SilentInstallation::_unSelectRunningActions;	
};


TEST(SilentInstallationTest, _selectRunningActionsTest_KeepSelected)
{
	SilentInstallationTest silentInstallation;
	SilentInstallationAction action;
	vector <Action *> actions;
	ExecutionProcess executionProcess;
	
	EXPECT_CALL(action, GetStatus()).WillRepeatedly(Return(Selected));
	EXPECT_CALL(action, GetName()).WillRepeatedly(Return((wchar_t*) L"Action name"));
	EXPECT_CALL(action, GetExecutingProcess()).WillRepeatedly(Return(executionProcess));
	EXPECT_CALL(action, SetStatus(_)).Times(0);

	actions.push_back(&action);
	silentInstallation._unSelectRunningActions(&actions);	
}

TEST(SilentInstallationTest, _selectRunningActionsTest_UnSelected)
{
	SilentInstallationTest silentInstallation;
	SilentInstallationAction action;
	vector <Action *> actions;
	ExecutionProcess executionProcess(L"test process name", L"test process printable name", true);

	EXPECT_CALL(action, GetStatus()).WillRepeatedly(Return(Selected));
	EXPECT_CALL(action, GetName()).WillRepeatedly(Return((wchar_t*) L"Action name"));
	EXPECT_CALL(action, GetExecutingProcess()).WillRepeatedly(Return(executionProcess));

	EXPECT_CALL(action, SetStatus(NotSelected)).Times(1);
	actions.push_back(&action);
	silentInstallation._unSelectRunningActions(&actions);
}