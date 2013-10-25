/* 
 * Copyright (C) 2012 Xavier Ivars-Ribes <xavi.ivars@gmail.com>
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

#pragma once

#include <vector>
#include <algorithm>

using namespace std;

class ChromeProfile
{
public:
	ChromeProfile();
	ChromeProfile(wstring);

	bool WriteUILocale();
	virtual bool IsUiLocaleOk();
	virtual bool IsAcceptLanguagesOk();
	bool UpdateAcceptLanguages();

	void SetPath(wstring);
	bool ReadLanguageCode(wstring& langcode);
	
protected:
	void ParseLanguage(wstring regvalue);
	void CreateJSONString(wstring &regvalue);
	void AddCatalanToArrayAndRemoveOldIfExists();
	virtual wstring GetUIRelPathAndFile() { return L"/../User Data/Local State"; }
	virtual wstring GetPreferencesRelPathAndFile() { return L"/../User Data/Default/Preferences"; }

private: 		

	bool _isInstalled();

	bool _findIntl(wstring,int&);
	bool _findSemicolon(wstring,int&);
	bool _findStartBlock(wstring,int&);
	bool _findAcceptedKey(wstring,int&);
	bool _findAcceptedValue(wstring,int&);
	bool _findLanguageString(wstring,int &,wstring &);
	bool _findAppLocaleKey(wstring line, int & pos);

	void _getFirstLanguage(wstring& regvalue);
	
	bool _writeAcceptLanguageCode(wstring langcode);
	
	wstring m_installLocation;
	vector<wstring> m_languages;
};
