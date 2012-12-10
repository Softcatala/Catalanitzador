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
#include "Application.h"
#include "LogExtractor.h"

using ::testing::StrCaseEq;

void GetLpkSetupLogLocation(wstring& location)
{
	Application::GetExecutionLocation(location);
	location += L"LogExtractor\\lpksetup-20121201-132302-0.log";
}

void GetWLSetupLogLocation(wstring& location)
{
	Application::GetExecutionLocation(location);
	location += L"LogExtractor\\2012-12-08_14-00_1b90-p2b8xi9u.log";
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeyword7Lines)
{
	const wchar_t* KEYWORD = L"Error";
	const int LINES = 7;
	wstring file;

	GetLpkSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);	
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(7, lines.size());
	EXPECT_THAT(lines.at(0), StrCaseEq(L"13:23:05:586 : License \"Kernel-MUI-Language-SKU\" queried, value: \"en-US;en-GB;ar-SA;pt-BR;zh-TW;zh-CN;zh-HK;cs-CZ;da-DK;el-GR;es-ES;fi-FI;fr-FR;de-DE;he-IL;hu-HU;it-IT;ja-JP;ko-KR;nl-NL;nb-NO;pl-PL;pt-PT;ru-RU;sv-SE;tr-TR;bg-BG;hr-HR;et-EE;lv-LV;lt-LT;ro-RO;sr-Latn-CS;sk-SK;sl-SI;th-TH;uk-UA;fy-NL;qps-ploc;qps-plocm\""));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"13:23:05:601 : Language ca-ES has been disabled for the following reason: 2148468483"));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"13:23:05:601 : Failed to validate languages"));
	EXPECT_THAT(lines.at(3), StrCaseEq(L"13:23:05:601 : Error found in call to ProcessUnattendedArguments: 0x8007065B"));
	EXPECT_THAT(lines.at(4), StrCaseEq(L"13:23:05:601 : INFO: Session 0xbb1014 finalized, reporting no required reboot"));
	EXPECT_THAT(lines.at(5), StrCaseEq(L"13:23:05:601 : INFO: destroying CbsSession 0xbb1014"));
	EXPECT_THAT(lines.at(6), StrCaseEq(L"13:23:05:601 : INFO: attempt to re-finalize session 0xbb1014 not acted on"));	
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeyword5Lines)
{
	const wchar_t* KEYWORD = L"Error";
	const int LINES = 5;
	wstring file;

	GetLpkSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);	
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(5, lines.size());	
	EXPECT_THAT(lines.at(0), StrCaseEq(L"13:23:05:601 : Language ca-ES has been disabled for the following reason: 2148468483"));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"13:23:05:601 : Failed to validate languages"));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"13:23:05:601 : Error found in call to ProcessUnattendedArguments: 0x8007065B"));
	EXPECT_THAT(lines.at(3), StrCaseEq(L"13:23:05:601 : INFO: Session 0xbb1014 finalized, reporting no required reboot"));
	EXPECT_THAT(lines.at(4), StrCaseEq(L"13:23:05:601 : INFO: destroying CbsSession 0xbb1014"));	
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeywordEnd)
{
	const wchar_t* KEYWORD = L"ExitProcess";
	const int LINES = 7;
	wstring file;

	GetLpkSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(4, lines.size());
	EXPECT_THAT(lines.at(0), StrCaseEq(L"13:23:05:633 : PERF: RestorePointEnd - ENTER"));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"13:23:05:633 : PERF: RestorePointEnd - LEAVE"));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"13:23:05:633 : DEBUG: Cleaning working path in a new process"));
	EXPECT_THAT(lines.at(3), StrCaseEq(L"13:23:05:633 : ExitProcess: There was an internal error: 0x8007065B"));
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeywordDoesNotExist)
{
	const wchar_t* KEYWORD = L"KeywordDoesNotExists";
	const int LINES = 7;
	wstring file;

	GetLpkSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();
	EXPECT_EQ(0, lines.size());
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeyword7Lines_Ascii)
{
	const wchar_t* KEYWORD = L"Error";
	const int LINES = 7;
	wstring file;

	GetWLSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);	
	logExtractor.SetFileIsUnicode(false);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(5, lines.size());
	EXPECT_THAT(lines.at(0), StrCaseEq(L"SingleInstance :000014B0 (12/08/2012 14:00:49.386) Existing server found"));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"Cert           :000014B0 (12/08/2012 14:00:49.386) Verifying signature of file=[C:\\Program Files\\Windows Live\\Installer\\wlarp.exe]..."));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"Exe            :000014B0 (12/08/2012 14:00:49.438) Detected another running instance of the installer"));
	EXPECT_THAT(lines.at(3), StrCaseEq(L"!ERROR!        :000014B0 (12/08/2012 14:02:01) SOURCE=Exe, CODE=0x80280006 SetupUX reported fatal error (0x80280006)"));
	EXPECT_THAT(lines.at(4), StrCaseEq(L"Logger         :000014B0 (12/08/2012 14:02:05.283) Logger Shutdown (collection=Yes; upload=Yes; InternalLogUpload=Yes; maxcabsize=300 Kb)"));
}


TEST(LogExtractorTest, ExtractLogFragmentForKeyword_LastOccurrenceInMiddle)
{
	const wchar_t* KEYWORD = L"ExeUtil";
	const int LINES = 3;
	wstring file;

	GetWLSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);
	logExtractor.SetFileIsUnicode(false);
	logExtractor.SetExtractLastOccurrence(true);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(3, lines.size());
	EXPECT_THAT(lines.at(0), StrCaseEq(L"LOG            :000014B0 (12/08/2012 14:00:49.158) Default Browser: firefox.exe"));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"ExeUtil        :000014B0 (12/08/2012 14:00:49.158) TrustLevel: REQUIRE: REAL"));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"InstallerConfig:000014B0 (12/08/2012 14:00:49.158) Loading configuration 'CONFIG0' from resource"));	
}

TEST(LogExtractorTest, ExtractLogFragmentForKeyword_ErrorKeyword5Lines_LastOccurrence)
{
	const wchar_t* KEYWORD = L"Error";
	const int LINES = 5;
	wstring file;

	GetLpkSetupLogLocation(file);
	LogExtractor logExtractor(file, LINES);
	logExtractor.SetExtractLastOccurrence(true);
	logExtractor.ExtractLogFragmentForKeyword(KEYWORD);
	const vector <wstring> & lines = logExtractor.GetLines();

	EXPECT_EQ(3, lines.size());
	EXPECT_THAT(lines.at(0), StrCaseEq(L"13:23:05:633 : PERF: RestorePointEnd - LEAVE"));
	EXPECT_THAT(lines.at(1), StrCaseEq(L"13:23:05:633 : DEBUG: Cleaning working path in a new process"));
	EXPECT_THAT(lines.at(2), StrCaseEq(L"13:23:05:633 : ExitProcess: There was an internal error: 0x8007065B"));
}