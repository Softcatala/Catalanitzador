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
#include "DownloadInetMock.h"

using ::testing::StrCaseEq;
using ::testing::StrCaseNe;
using ::testing::_;
using ::testing::Return;


TEST(DownloadManagerTest, GetFileSize)
{
	int EXPECTED_SIZE = 800;
	DownloadInetMock downloadInetMock;	
	DownloadManager downloadManager((IDownloadInet*) &downloadInetMock);

	EXPECT_CALL(downloadInetMock, GetFileSize(StrCaseEq(L"url1"))).WillRepeatedly(Return(0));
	EXPECT_CALL(downloadInetMock, GetFileSize(StrCaseEq(L"url2"))).WillRepeatedly(Return(0));
	EXPECT_CALL(downloadInetMock, GetFileSize(StrCaseEq(L"url3"))).WillRepeatedly(Return(EXPECTED_SIZE));

	ConfigurationFileActionDownload configurationFileActionDownload;
	configurationFileActionDownload.AddUrl(L"url1");
	configurationFileActionDownload.AddUrl(L"url2");
	configurationFileActionDownload.AddUrl(L"url3");
	
	EXPECT_THAT(downloadManager.GetFileSize(configurationFileActionDownload), EXPECTED_SIZE);
}

TEST(DownloadManagerTest, GetFile)
{
	TempFile tempFile;
	DownloadInetMock downloadInetMock;	
	DownloadManager downloadManager((IDownloadInet*) &downloadInetMock);
	HANDLE hFile;
	Sha1Sum sha1sum;
	DWORD dwWritten;
	
	wchar_t* STRING = L"123456789A";	
	char* STRING_SHA1 ="c5c98e82ca47ab1adc7f05085e22366de620d94e";

	hFile = CreateFile(tempFile.GetFileName().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, STRING, wcslen(STRING) * sizeof(wchar_t), &dwWritten, NULL);
	CloseHandle(hFile);	

	wstring sha1file(tempFile.GetFileName() + L".sha1");
	hFile = CreateFile(sha1file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, STRING_SHA1, strlen(STRING_SHA1) * sizeof(wchar_t), &dwWritten, NULL);
	CloseHandle(hFile);
	
	EXPECT_CALL(downloadInetMock, GetFile(StrCaseEq(L"url1"),_,_,_)).WillRepeatedly(Return(false));
	EXPECT_CALL(downloadInetMock, GetFile(StrCaseEq(L"url2"),_,_,_)).WillRepeatedly(Return(false));
	EXPECT_CALL(downloadInetMock, GetFile(StrCaseEq(L"url3"),_,_,_)).WillRepeatedly(Return(true));
	EXPECT_CALL(downloadInetMock, GetFile(StrCaseEq(L"url3.sha1"),_,_,_)).WillRepeatedly(Return(true));
	
	ConfigurationFileActionDownload configurationFileActionDownload;
	configurationFileActionDownload.AddUrl(L"url1");
	configurationFileActionDownload.AddUrl(L"url2");
	configurationFileActionDownload.AddUrl(L"url3");

	EXPECT_TRUE(downloadManager.GetFileAndVerifyAssociatedSha1(configurationFileActionDownload, tempFile.GetFileName(), NULL, NULL));	
}
