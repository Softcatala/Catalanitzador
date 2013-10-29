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

class AcceptLanguagePropertyValue
{

public:

	AcceptLanguagePropertyValue(wstring value);
	wstring GetWithCatalanAdded();
	wstring GetFirstLanguage();

private:

	wstring _createJSONString(vector<wstring> languages);
	void _addCatalanToArrayAndRemoveOldIfExists(vector<wstring>& languages);
	vector<wstring> _getLanguagesFromAcceptString(wstring value);

	wstring value;
};

class ChromeProfile
{
public:
	ChromeProfile();

	bool WriteUILocale();
	virtual bool IsUiLocaleOk();

	virtual bool IsAcceptLanguagesOk();
	void SetAcceptLanguages();
	bool ReadAcceptLanguages(wstring& langcode);

	bool IsSpellCheckerLanguageOk();
	void SetSpellCheckerLanguage();
	bool WriteSpellAndAcceptLanguages();

	void SetPath(wstring);
	
protected:

	virtual wstring GetUIRelPathAndFile() { return L"/../User Data/Local State"; }
	virtual wstring GetPreferencesRelPathAndFile() { return L"/../User Data/Default/Preferences"; }

private: 		

	vector<wstring> _getLanguagesFromAcceptString(wstring value);

	bool _findIntl(wstring,int&);
	bool _findSemicolon(wstring,int&);
	bool _findStartBlock(wstring,int&);
	bool _findLanguageString(wstring,int &,wstring &);	
	bool _findProperty(wstring line, wstring key, int & pos);
	bool _readSchema(wstring line, int & pos);
	bool _readPropertyValue(wstring line, wstring key, int& pos, wstring& value);
	void _getFirstLanguage(wstring& regvalue);

	void _readAcceptAndSpellLanguagesFromPreferences();
	
	wstring m_installLocation;	
	wstring m_spellLanguageToSet;
	bool m_setCatalanLanguage;

	wstring m_prefCacheAcceptLanguage;
	wstring m_prefCacheSpellLanguage;	
	bool m_prefCacheIsValid;
};
