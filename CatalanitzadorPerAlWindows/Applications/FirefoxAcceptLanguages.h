/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
#include "FirefoxPreferencesFile.h"

using namespace std;

class FirefoxAcceptLanguages : FirefoxPreferencesFile
{
public:		
		FirefoxAcceptLanguages(wstring profileRootDir, wstring locale);

		bool Execute();
		bool IsNeed();	
		bool ReadLanguageCode();
		void InvalidateCache() {m_CachedLanguageCode = false; }

protected:

		wstring* _getLocale() {return &m_locale;}
		vector <wstring> * _getLanguages() {return &m_languages;}
		
private:

		void _getFirstLanguage(wstring& regvalue);		
		void _createPrefsString(wstring& string);
		void _addCatalanToArrayAndRemoveOldIfExists();
		bool _writeLanguageCode(wstring &langcode);
		void _getPrefLine(wstring langcode, wstring& line);
		void _addFireForLocale();
		void _parseLanguage(wstring regvalue);
		vector <wstring> m_languages;
		wstring m_locale;

		bool m_CachedLanguageCode;		
};

