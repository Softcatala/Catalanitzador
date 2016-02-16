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

#include "LanguageList.h"
#include "ActionExecution.h"


LanguageList::LanguageList(wstring _value)
{
	m_value = _value;
	m_separator = L',';
	m_catalanLangCode = L"ca";
}

wstring LanguageList::GetWithCatalanAdded()
{
	vector<wstring> languages;
	
	languages = _getLanguagesFromAcceptString(m_value);
	_addCatalanToArrayAndRemoveOldIfExists(languages);
	return _createJSONString(languages);
}

wstring LanguageList::_createJSONString(vector<wstring> languages)
{	
	if (languages.size() == 1)
	{
		return languages.at(0);
	}

	wstring jsonvalue;
	jsonvalue = languages.at(0);
		
	for (unsigned int i = 1; i < languages.size(); i++)
	{
		jsonvalue += m_separator + languages.at(i);
	}
	return jsonvalue;
}

vector<wstring> LanguageList::_getLanguagesFromAcceptString(wstring value)
{
	wstring language;
	vector<wstring> languages;	
	
	for (unsigned int i = 0; i < value.size (); i++)
	{
		if (value[i] == m_separator)
		{
			languages.push_back(language);
			language.clear();
		} else {
			language += value[i];
		}
	}

	if (language.empty() == false)
	{
		languages.push_back(language);
	}
	return languages;
}

wstring LanguageList::GetFirstLanguage()
{
	wstring jsonvalue;
	vector<wstring> languages;

	languages = _getLanguagesFromAcceptString(m_value);

	if(languages.size() > 0)
		jsonvalue = languages[0];	

	return jsonvalue;
}

void LanguageList::_addCatalanToArrayAndRemoveOldIfExists(vector<wstring>& languages)
{	
	wstring regvalue;	
	vector<wstring>::iterator it;

	// Delete previous occurrences of Catalan locale that were not first
	for (it = languages.begin(); it != languages.end(); ++it)
	{
		wstring element = *it;
		std::transform(element.begin(), element.end(), element.begin(), ::tolower);

		if (element.compare(m_catalanLangCode) == 0)
		{
			languages.erase(it);
			break;
		}
	}
	
	it = languages.begin();
	languages.insert(it, m_catalanLangCode);
}