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
#include "ConfigurationRemote.h"
#include "ConfigurationInstance.h"
#include "ConfigurationFileActionDownload.h"
#include "ConfigurationFileActionDownloads.h"


using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;

#define CreateCatalanitzadorUpdate \
	DownloadManager downloadManager; \
	RunnerMock runnerMock; \
	CatalanitzadorUpdateAction catalanitzadorAction(&runnerMock, &downloadManager);

#define VERSION L"3.2.1"

class CatalanitzadorUpdateActionTest: public testing::Test
{
protected:

	virtual void TearDown()
	{
		ConfigurationInstance::Reset();
	}
};

void _configureCatalanitzadorInDownloadFile(wstring version_req)
{
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload, fileActionDownloadResult;

	fileActionDownload.SetMinVersion(ApplicationVersion(L"0.0.0"));
	fileActionDownload.SetMaxVersion(ApplicationVersion(version_req));
	fileActionDownloads.SetActionID(CatalanitzadorUpdateActionID);
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);
	ConfigurationInstance::Get().SetRemote(remote);
}

TEST_F(CatalanitzadorUpdateActionTest, SetGetVersion)
{
	CreateCatalanitzadorUpdate;

	catalanitzadorAction.SetVersion(VERSION);
	EXPECT_THAT(catalanitzadorAction.GetVersion(), StrCaseEq(VERSION));
}

TEST_F(CatalanitzadorUpdateActionTest, Execute)
{
	CreateCatalanitzadorUpdate;

	catalanitzadorAction.SetVersion(VERSION);
	
	EXPECT_CALL(runnerMock, Execute(_, HasSubstr(L"/NoRunningCheck:3.2.1"), false)).
		Times(1).WillRepeatedly(Return(true));

	catalanitzadorAction.Execute();
}

TEST_F(CatalanitzadorUpdateActionTest, CheckPrerequirements_AlreadyApplied)
{
	const wchar_t* VERSION_REQ = L"1.2.3";
	CreateCatalanitzadorUpdate;
	
	_configureCatalanitzadorInDownloadFile(VERSION_REQ);
	ConfigurationInstance::Get().SetVersion(ApplicationVersion(VERSION_REQ));
	catalanitzadorAction.CheckPrerequirements(NULL);

	EXPECT_THAT(catalanitzadorAction.GetStatus(), AlreadyApplied);
	EXPECT_FALSE(catalanitzadorAction.IsNeed());
}

TEST_F(CatalanitzadorUpdateActionTest, CheckPrerequirements_AlreadyApplied_Successful)
{
	const wchar_t* VERSION_REQ = L"1.2.3";
	CreateCatalanitzadorUpdate;
	
	catalanitzadorAction.SetStatus(Successful);
	_configureCatalanitzadorInDownloadFile(VERSION_REQ);
	ConfigurationInstance::Get().SetVersion(ApplicationVersion(VERSION_REQ));
	catalanitzadorAction.CheckPrerequirements(NULL);

	EXPECT_THAT(catalanitzadorAction.GetStatus(), Successful);
}

TEST_F(CatalanitzadorUpdateActionTest, CheckPrerequirements_IsNeeded)
{
	const wchar_t* VERSION_LATEST = L"1.2.3";
	const wchar_t* VERSION_PROG = L"1.2.0";

	CreateCatalanitzadorUpdate;
	
	_configureCatalanitzadorInDownloadFile(VERSION_LATEST);	
	ConfigurationInstance::Get().SetVersion(ApplicationVersion(VERSION_PROG));	
	catalanitzadorAction.CheckPrerequirements(NULL);
	
	EXPECT_TRUE(catalanitzadorAction.IsNeed());
}

