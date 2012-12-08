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
