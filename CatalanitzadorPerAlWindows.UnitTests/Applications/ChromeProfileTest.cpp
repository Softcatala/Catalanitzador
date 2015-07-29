/* 
 * Copyright (C) 2012-2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "ChromeProfile.h"
#include "Application.h"

using ::testing::Return;
using ::testing::_;
using ::testing::StrCaseEq;

class ChomeProfileTest : public ChromeProfile
{
public:	

	ChomeProfileTest() : ChromeProfile() {};

	void SetUIRelPathAndFile(wstring pathfile) { m_ui_file = pathfile; }
	void SetPreferencesRelPathAndFile(wstring pathfile) { m_preferences_file = pathfile; }

	virtual wstring GetUIRelPathAndFile() 
	{
		if (m_ui_file.empty())
			return ChromeProfile::GetUIRelPathAndFile();
		else
			return m_ui_file;
	}

	virtual wstring GetPreferencesRelPathAndFile() 
	{
		if (m_preferences_file.empty())
			return ChromeProfile::GetPreferencesRelPathAndFile();
		else
			return m_preferences_file;		
	}

private:

	wstring m_ui_file;
	wstring m_preferences_file;
};

wstring GetFileNameFromFullFileName(wstring path)
{
	wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_wsplitpath_s(path.c_str(), drive, dir, fname, ext);
	wstring file = fname;
	file += ext;
	return file;
}

wstring GetPathFromFullFileName(wstring path)
{
	wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_wsplitpath_s(path.c_str(), drive, dir, fname, ext);
	wstring file = drive;
	file += dir;
	return file;
}

TEST(ChomeProfileTest, None_SetPath)
{
	wstring langcode;
	ChomeProfileTest chromeProfile;
	chromeProfile.SetPath(L"");

	EXPECT_FALSE(chromeProfile.IsUiLocaleOk());
	EXPECT_FALSE(chromeProfile.ReadAcceptLanguages(langcode));
	EXPECT_FALSE(chromeProfile.WriteSpellAndAcceptLanguages());
	EXPECT_FALSE(chromeProfile.WriteUILocale());
}

TEST(ChomeProfileTest, SpanishUI_IsUiLocaleOk)
{
	wstring location;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";
	chromeProfile.SetPath(location);

	EXPECT_FALSE(chromeProfile.IsUiLocaleOk());
}

TEST(ChomeProfileTest, CatatanUI_IsUiLocaleOk)
{
	wstring location;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\CatalanUI_NoAcceptLanguage\\User Data\\";
	chromeProfile.SetPath(location);

	EXPECT_TRUE(chromeProfile.IsUiLocaleOk());
}

TEST(ChomeProfileTest, AcceptLanguage_es_de_br_ReadAcceptLanguages)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_de_br\\User Data\\";
	chromeProfile.SetPath(location);

	bool bRslt = chromeProfile.ReadAcceptLanguages(langcode);
	EXPECT_TRUE(bRslt);
	EXPECT_THAT(langcode, StrCaseEq(L"es,de,br"));
}

TEST(ChomeProfileTest, NoAcceptLanguage_ReadAcceptLanguages)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";
	chromeProfile.SetPath(location);

	bool bRslt = chromeProfile.ReadAcceptLanguages(langcode);
	EXPECT_FALSE(bRslt);
	EXPECT_THAT(langcode, StrCaseEq(L""));
}

TEST(ChomeProfileTest, NoFile_ReadAcceptLanguages)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);	
	chromeProfile.SetPath(location);

	bool bRslt = chromeProfile.ReadAcceptLanguages(langcode);
	EXPECT_FALSE(bRslt);
	EXPECT_THAT(langcode, StrCaseEq(L""));
}

TEST(ChomeProfileTest, WriteUILocale)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";
	location += L"/../User Data/Local State";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);
	
	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetUIRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	chromeProfile.WriteUILocale();
	EXPECT_TRUE(chromeProfile.IsUiLocaleOk());	
}

TEST(ChomeProfileTest, NoPrevLanguage_SetAcceptLanguages)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";
	location += L"/../User Data/Default/Preferences";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);
	
	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetPreferencesRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	chromeProfile.SetCatalanAsAcceptLanguages();
	chromeProfile.WriteSpellAndAcceptLanguages();
	chromeProfile.ReadAcceptLanguages(langcode);
	EXPECT_THAT(langcode, StrCaseEq(L"ca"));
}

TEST(ChomeProfileTest, PrevLanguage_SetAcceptLanguages)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_de_br\\User Data\\";
	location += L"/../User Data/Default/Preferences";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);
	
	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetPreferencesRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	chromeProfile.SetCatalanAsAcceptLanguages();
	chromeProfile.WriteSpellAndAcceptLanguages();
	chromeProfile.ReadAcceptLanguages(langcode);
	EXPECT_THAT(langcode, StrCaseEq(L"ca,es,de,br"));
}

TEST(ChomeProfileTest, NoAcceptLangLocaleCa_IsAcceptLanguagesOk)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_de_br\\User Data\\";
	chromeProfile.SetPath(location);	
	EXPECT_FALSE(chromeProfile.IsAcceptLanguagesOk());
}

TEST(ChomeProfileTest, NonCaAcceptLangLocaleLocaleUIOk_IsAcceptLanguagesOk)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\CatalanUI_NoAcceptLanguage\\User Data\\";
	chromeProfile.SetPath(location);	
	EXPECT_TRUE(chromeProfile.IsAcceptLanguagesOk());
}

TEST(ChomeProfileTest, NonCaAcceptLangLocaleLocaleUINonOk_IsAcceptLanguagesOk)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_NoAcceptLanguage\\User Data\\";
	chromeProfile.SetPath(location);	
	EXPECT_FALSE(chromeProfile.IsAcceptLanguagesOk());
}

TEST(ChomeProfileTest, CaAcceptLangLocaleFirst_IsAcceptLanguagesOk)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_ca\\User Data\\";
	chromeProfile.SetPath(location);	
	EXPECT_TRUE(chromeProfile.IsAcceptLanguagesOk());
}

TEST(ChomeProfileTest, CaAcceptLangLocaleNonFirst_IsAcceptLanguagesOk)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_ca\\User Data\\";
	chromeProfile.SetPath(location);	
	EXPECT_FALSE(chromeProfile.IsAcceptLanguagesOk());
}

TEST(ChomeProfileTest, CaSpellChecker_IsSpellCheckerLanguageOk)
{
	wstring location;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_ca\\User Data\\";
	chromeProfile.SetPath(location);

	EXPECT_TRUE(chromeProfile.IsSpellCheckerLanguageOk());
}

TEST(ChomeProfileTest, EsSpellChecker_IsSpellCheckerLanguageOk)
{
	wstring location;
	ChomeProfileTest chromeProfile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_ca\\User Data\\";
	chromeProfile.SetPath(location);

	EXPECT_FALSE(chromeProfile.IsSpellCheckerLanguageOk());
}

TEST(ChomeProfileTest, EsSpellChecker_SetCatalanAsSpellCheckerLanguage)
{
	wstring location;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\SpanishUI_AcceptLanguage_es_ca\\User Data\\";
	location += L"/../User Data/Default/Preferences";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);

	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetPreferencesRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	chromeProfile.SetCatalanAsSpellCheckerLanguage();
	chromeProfile.WriteSpellAndAcceptLanguages();

	EXPECT_TRUE(chromeProfile.IsSpellCheckerLanguageOk());
}

TEST(ChomeProfileTest, OneLineLocalState_ReadUILocale)
{
	wstring location;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\OneLineProfile\\User Data\\";
	location += L"/../User Data/Local State";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);

	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetUIRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	EXPECT_TRUE(chromeProfile.IsUiLocaleOk());
}

TEST(ChomeProfileTest, OneLineLocalState_WriteUILocale)
{
	wstring location, langcode;
	ChomeProfileTest chromeProfile;
	TempFile tempFile;

	Application::GetExecutionLocation(location);
	location += L"Chrome\\OneLineProfile\\User Data\\";
	location += L"/../User Data/Local State";
	CopyFile(location.c_str(), tempFile.GetFileName().c_str(), false);
	
	chromeProfile.SetPath(GetPathFromFullFileName(tempFile.GetFileName()));
	chromeProfile.SetUIRelPathAndFile(GetFileNameFromFullFileName(tempFile.GetFileName()));

	chromeProfile.WriteUILocale();
	EXPECT_TRUE(chromeProfile.IsUiLocaleOk());	
}
