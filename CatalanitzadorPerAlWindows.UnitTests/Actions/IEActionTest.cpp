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
#include "IEAction.h"
#include "IELPIAction.h"
#include "IEAcceptLanguagesAction.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::DoAll;

#define CreateIEActionAction \
	ActionMock LPIActionMock; \
	ActionMock acceptLanguagesMock; \
	IEAction action((IELPIAction* ) &LPIActionMock, (IEAcceptLanguagesAction*)&acceptLanguagesMock);\


#define DEFAULT_ACTION_STATUS NotSelected

struct IEActionStatuses
{
	ActionStatus lpiStatus;
	ActionStatus acceptLanguageStatus;
	ActionStatus expectedStatus;
} IEActionStatusesTest[] =
{
	{AlreadyApplied, AlreadyApplied, AlreadyApplied},
	{AlreadyApplied, Selected, DEFAULT_ACTION_STATUS},
	{AlreadyApplied, CannotBeApplied, CannotBeApplied},
	
	{CannotBeApplied, AlreadyApplied, CannotBeApplied},
	{CannotBeApplied, Selected, DEFAULT_ACTION_STATUS},
	{CannotBeApplied, CannotBeApplied, CannotBeApplied},

	{Selected, AlreadyApplied, DEFAULT_ACTION_STATUS},
	{Selected, CannotBeApplied, DEFAULT_ACTION_STATUS},
	{Selected, Selected, DEFAULT_ACTION_STATUS},
};

void _setMockCheckPrerequirements(ActionMock& LPIActionMock, ActionMock& acceptLanguagesMock)
{	
	EXPECT_CALL(LPIActionMock, CheckPrerequirements(_));
	EXPECT_CALL(acceptLanguagesMock, CheckPrerequirements(_));
	EXPECT_CALL(LPIActionMock, IsNeed()).WillRepeatedly(Return(false));
	EXPECT_CALL(acceptLanguagesMock, IsNeed()).WillRepeatedly(Return(false));
}

TEST(IEActionTest, CheckPrerequirements_Status)
{
	const int size =  sizeof(IEActionStatusesTest) / sizeof(IEActionStatusesTest[0]);
	for (int i = 0; i < size; i++)
	{
		CreateIEActionAction
		IEActionStatuses actionStatuses = IEActionStatusesTest[i];

		EXPECT_CALL(LPIActionMock, GetStatus()).WillRepeatedly(Return(actionStatuses.lpiStatus));
		EXPECT_CALL(acceptLanguagesMock, GetStatus()).WillRepeatedly(Return(actionStatuses.acceptLanguageStatus));
		_setMockCheckPrerequirements(LPIActionMock, acceptLanguagesMock);

		action.CheckPrerequirements(NULL);
		EXPECT_THAT(action.GetStatus(), actionStatuses.expectedStatus);
	}
}
