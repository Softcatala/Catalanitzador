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
#include "CatalanitzadorUpdateAction.h"
#include "RunnerMock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;

#define CreateCatalanitzadorUpdate \
	DownloadManager downloadManager; \
	RunnerMock runnerMock; \
	CatalanitzadorUpdateAction catalanitzadorAction(&runnerMock, &downloadManager);

#define VERSION L"3.2.1"

TEST(CatalanitzadorUpdateActionTest, SetGetVersion)
{
	CreateCatalanitzadorUpdate;

	catalanitzadorAction.SetVersion(VERSION);
	EXPECT_THAT(catalanitzadorAction.GetVersion(), StrCaseEq(VERSION));
}

TEST(CatalanitzadorUpdateActionTest, Execute)
{
	CreateCatalanitzadorUpdate;

	catalanitzadorAction.SetVersion(VERSION);
	
	EXPECT_CALL(runnerMock, Execute(_, HasSubstr(L"/NoRunningCheck:3.2.1"), false)).
		Times(1).WillRepeatedly(Return(true));

	catalanitzadorAction.Execute();
}