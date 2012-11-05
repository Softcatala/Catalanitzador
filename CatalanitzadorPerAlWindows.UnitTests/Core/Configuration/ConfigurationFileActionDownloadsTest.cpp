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
#include "ConfigurationFileActionDownloads.h"
#include "ActionID.h"

using ::testing::StrCaseEq;

TEST(ConfigurationFileActionDownloadsTest, GetSetActionID)
{	
	ConfigurationFileActionDownloads fileActionDownloads;
	fileActionDownloads.SetActionID(WindowsLPI);

	EXPECT_THAT(fileActionDownloads.GetActionID(), WindowsLPI);
}

#define ADD_VERSION L"1.9.1"
TEST(ConfigurationFileActionDownloadsTest, AddFileActionDownload)
{	
	ConfigurationFileActionDownload fileActionDownload, gotfileActionDownload;
	fileActionDownload.SetMinVersion(ApplicationVersion(ADD_VERSION));
	ConfigurationFileActionDownloads fileActionDownloads;
	fileActionDownloads.AddFileActionDownload(fileActionDownload);

	gotfileActionDownload = (ConfigurationFileActionDownload) fileActionDownloads.GetFileActionDownloadCollection().at(0);
	EXPECT_TRUE(gotfileActionDownload.GetMinVersion() == fileActionDownload.GetMinVersion());
}


#define DOWN1_MIN_VERSION L"1.2.0"
#define DOWN1_MAX_VERSION L"1.2.9"
#define DOWN1_MAX_EMPTY1  L"1.1.0"
#define DOWN1_MAX_EMPTY2  L"1.3.0"

TEST(ConfigurationFileActionDownloadsTest, GetFileDownloadForVersion_Empty)
{
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload, fileActionDownloadResult;

	fileActionDownload.SetMinVersion(ApplicationVersion(DOWN1_MIN_VERSION));
	fileActionDownload.SetMaxVersion(ApplicationVersion(DOWN1_MAX_VERSION));
	fileActionDownloads.AddFileActionDownload(fileActionDownload);

	fileActionDownloadResult = fileActionDownloads.GetFileDownloadForVersion(ApplicationVersion(DOWN1_MAX_EMPTY1));

	EXPECT_TRUE(fileActionDownloadResult.IsEmpty());
}

#define DOWN1_MACH  L"1.2.2"

TEST(ConfigurationFileActionDownloadsTest, GetFileDownloadForVersion_Match)
{
	ConfigurationFileActionDownloads fileActionDownloads;
	ConfigurationFileActionDownload fileActionDownload, fileActionDownloadResult;

	fileActionDownload.SetMinVersion(ApplicationVersion(DOWN1_MIN_VERSION));
	fileActionDownload.SetMaxVersion(ApplicationVersion(DOWN1_MAX_VERSION));
	fileActionDownloads.AddFileActionDownload(fileActionDownload);

	fileActionDownloadResult = fileActionDownloads.GetFileDownloadForVersion(ApplicationVersion(DOWN1_MACH));
	EXPECT_FALSE(fileActionDownloadResult.IsEmpty());
	EXPECT_TRUE(fileActionDownloadResult.GetMaxVersion() == fileActionDownload.GetMaxVersion());
}

