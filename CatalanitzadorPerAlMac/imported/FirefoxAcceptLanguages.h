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

#pragma once

#include "Action.h"

#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class FirefoxAcceptLanguages
{
public:
	FirefoxAcceptLanguages(string profileRootDir, string locale);
	
	bool Execute();
	bool IsNeed();
	bool ReadLanguageCode();
	
protected:
	
	string* _getLocale() {return &m_locale;}
	vector <string> * _getLanguages() {return &m_languages;}
	
private:
	
	void _getFirstLanguage(string& regvalue);
	string _getProfileRootDir() {return m_profileRootDir;}
	void _createPrefsString(string& string);
	void _addCatalanToArrayAndRemoveOldIfExists();
	bool _writeLanguageCode(string &langcode);
	void _getPrefLine(string langcode, string& line);
	void _addFireForLocale();
	void _parseLanguage(string regvalue);
	
	void _getProfilesIniLocation(string &location);
	bool _getPreferencesFile(string &location);
	bool _getProfileLocationFromProfilesIni(string file, string &profileLocation);
	
	vector <string> m_languages;
	string m_locale;
	string m_profileRootDir;
	bool m_CachedLanguageCode;
	
};

