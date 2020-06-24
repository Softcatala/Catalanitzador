/* 
 * Copyright (C) 2015 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "FirefoxLangPackAction.h"
#include "FirefoxChannelMock.h"
#include "ConfigurationInstance.h"

using ::testing::Return;

class FirefoxLangPackActionTest : public testing::Test
{
public:
	
		virtual void TearDown()
		{
			ConfigurationInstance::Reset();
		}
};


class FirefoxLangPackActionForTest : public FirefoxLangPackAction
{
public:
		
	FirefoxLangPackActionForTest::FirefoxLangPackActionForTest(IRunner* runner, IFirefoxChannel* firefoxChannel, wstring locale, wstring version, bool is64Bits, DownloadManager* downloadManager)
		: FirefoxLangPackAction(runner, firefoxChannel, locale, version, is64Bits, downloadManager) {};

	public:
			using FirefoxLangPackAction::_isLocaleInstalled;
			using FirefoxLangPackAction::_isSupportedChannel;		
};

#define DEFAULT_VERSION L"1.00"

#define CreateFirefoxLangPackAction(LOCALE, IS64_BITS) \
	RunnerMock runnerMock; \
	FirefoxChannelMock firefoxChannelMock; \
	FirefoxLangPackActionForTest firefoxLangPackAction(&runnerMock, &firefoxChannelMock, LOCALE, DEFAULT_VERSION, IS64_BITS, &DownloadManager());

	
TEST_F(FirefoxLangPackActionTest, _isLocaleInstalled_True)
{
	CreateFirefoxLangPackAction(L"ca", false)
	EXPECT_TRUE(firefoxLangPackAction._isLocaleInstalled());
}

TEST_F(FirefoxLangPackActionTest, _isLocaleInstalledValencia_True)
{
	CreateFirefoxLangPackAction(L"ca-valencia", false)
	EXPECT_TRUE(firefoxLangPackAction._isLocaleInstalled());
}

TEST_F(FirefoxLangPackActionTest, _isLocaleInstalled_False)
{	
	CreateFirefoxLangPackAction(L"es", false)
	EXPECT_FALSE(firefoxLangPackAction._isLocaleInstalled());
}

TEST_F(FirefoxLangPackActionTest, _isSupportedChannel_True)
{
	CreateFirefoxLangPackAction(L"ca", false)

	EXPECT_CALL(firefoxChannelMock, GetChannel()).WillRepeatedly(Return(L"release"));
	EXPECT_TRUE(firefoxLangPackAction._isSupportedChannel());
}

TEST_F(FirefoxLangPackActionTest, _isSupportedChannel_Yes)
{
	CreateFirefoxLangPackAction(L"ca", false)

	EXPECT_CALL(firefoxChannelMock, GetChannel()).WillRepeatedly(Return(L"beta"));
	EXPECT_FALSE(firefoxLangPackAction._isSupportedChannel());
}

TEST_F(FirefoxLangPackActionTest, _CheckPrerequirements_AlreadyApplied)
{
	CreateFirefoxLangPackAction(L"ca", false)
	firefoxLangPackAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxLangPackAction.GetStatus(), AlreadyApplied);
}

TEST_F(FirefoxLangPackActionTest, _CheckPrerequirements_isSupportedChannel_False_CannotBeApplied)
{
	CreateFirefoxLangPackAction(L"es", false)
	EXPECT_CALL(firefoxChannelMock, GetChannel()).WillRepeatedly(Return(L"beta"));

	firefoxLangPackAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxLangPackAction.GetStatus(), CannotBeApplied);
}

TEST_F(FirefoxLangPackActionTest, _CheckPrerequirements_VersionWithNoDownload_CannotBeApplied)
{
	CreateFirefoxLangPackAction(L"es", false)
	EXPECT_CALL(firefoxChannelMock, GetChannel()).WillRepeatedly(Return(L"release"));

	ConfigurationRemote remote;
	ConfigurationInstance::Get().SetRemote(remote);

	firefoxLangPackAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxLangPackAction.GetStatus(), CannotBeApplied);
}

TEST_F(FirefoxLangPackActionTest, _CheckPrerequirements_Ok)
{
	CreateFirefoxLangPackAction(L"es", false)
	EXPECT_CALL(firefoxChannelMock, GetChannel()).WillRepeatedly(Return(L"release"));

	ConfigurationRemote remote;
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload;

	fileActionDownload.SetMaxVersion(ApplicationVersion(DEFAULT_VERSION));
	fileActionDownload.SetMinVersion(ApplicationVersion(DEFAULT_VERSION));
	fileActionDownloads.SetActionID(FirefoxActionID);
	fileActionDownload.AddUrl(L"http://www.softcatala.org/");
	fileActionDownloads.AddFileActionDownload(fileActionDownload);
	remote.AddFileActionDownloads(fileActionDownloads);
	ConfigurationInstance::Get().SetRemote(remote);

	firefoxLangPackAction.SetStatus(Selected);	
	firefoxLangPackAction.CheckPrerequirements(NULL);
	EXPECT_THAT(firefoxLangPackAction.GetStatus(), Selected);
}
