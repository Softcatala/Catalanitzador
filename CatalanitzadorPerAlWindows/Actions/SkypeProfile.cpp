/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "SkypeProfile.h"
#include <algorithm>

SkypeProfile::SkypeProfile() 
{	

}

void SkypeProfile::_getSharedFile(wstring& location)
{
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Skype\\shared.xml";
}

void SkypeProfile::_getConfigFile(wstring& location, const wstring account)
{
	wchar_t szPath[MAX_PATH];
	
	SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE,  NULL, 0, szPath);
	location = szPath;
	location += L"\\Skype\\";
	location += account;
	location += L"\\config.xml";
}

enum AccountDefaultState
{
	AccountDefaultItemOther,
	AccountDefaultLib,
	AccountDefaultAccount
};

AccountDefaultState g_state = AccountDefaultItemOther;

bool SkypeProfile::_readDefaultAccountNodeCallback(XmlNode node, void *data)
{
	wstring name = node.GetName();

	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	switch (g_state)
	{
		case AccountDefaultItemOther:
			if (name == L"lib")
			{
				g_state = AccountDefaultLib;
			}
			return true;
		case AccountDefaultLib:
			if (name == L"account")
			{
				g_state = AccountDefaultAccount;
			}
			return true;
		case AccountDefaultAccount:
			{
				wstring* str = (wstring *)data;
				*str = node.GetText();
				g_state = AccountDefaultItemOther;
				return false;
			}
		default:
			return true;
	}
}

bool SkypeProfile::_readDefaultAccount(wstring& account)
{
	XmlParser parser;
	wstring file;	

	_getSharedFile(file);

	if (parser.Load(file) == false)
	{
		g_log.Log(L"SkypeProfile::_readDefaultAccount. Could not open '%s'", (wchar_t *) file.c_str());
		return false;
	}
	parser.Parse(_readDefaultAccountNodeCallback, &account);
	return account.size() > 0;
}

enum AccountDefaultLanguageState
{
	DefaultLanguageItemOther,
	DefaultLanguageGeneral,	
	DefaultLanguageLanguage
};

AccountDefaultLanguageState g_languageState = DefaultLanguageItemOther;

bool SkypeProfile::_readDefaultAccountLanguageNodeCallback(XmlNode node, void *data)
{	
	wstring name = node.GetName();

	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	switch (g_languageState)
	{
		case DefaultLanguageItemOther:
			if (name == L"general")
			{
				g_languageState = DefaultLanguageGeneral;				
			}
			return true;
		case DefaultLanguageGeneral:
			if (name == L"language")
			{
				wstring* str = (wstring *)data;
				*str = node.GetText();
				std::transform(str->begin(), str->end(), str->begin(), ::tolower);
				g_languageState = DefaultLanguageItemOther;
				return false;
			}
			return true;
		default:
			return true;
	}
}

#define CATALAN_LANG_PROFILE L"ca"

bool SkypeProfile::_readDefaultAccountLanguage(wstring& language)
{
	XmlParser parser;
	wstring file, account;

	if (_readDefaultAccount(account) == false)
		return false;
	
	_getConfigFile(file, account);
	if (parser.Load(file) == false)
	{
		g_log.Log(L"SkypeProfile::_readDefaultAccountLanguage. Could not open '%s'", (wchar_t *) file.c_str());
		return false;
	}

	parser.Parse(_readDefaultAccountLanguageNodeCallback, &language);
	return language.size() >0;
}

bool SkypeProfile::IsDefaultAccountLanguage()
{
	wstring language;

	_readDefaultAccountLanguage(language);
	return language == CATALAN_LANG_PROFILE;
}

TriBool SkypeProfile::CanReadDefaultProfile()
{
	if (m_canReadDefaultProfile.IsUndefined())
	{
		wstring language;

		m_canReadDefaultProfile = _readDefaultAccountLanguage(language) == true;
	}

	return m_canReadDefaultProfile;
}

void SkypeProfile::SetDefaultLanguage()
{
	bool bRslt;
	XmlParser parser;
	wstring file, account;

	if (_readDefaultAccount(account) == false)
		return;
	
	_getConfigFile(file, account);
	if (parser.Load(file) == false)
	{
		g_log.Log(L"SkypeProfile::SetDefaultLanguage. Could not open '%s'", (wchar_t *) file.c_str());
		return;
	}

	XmlNode currentNode(parser.getDocument());
	bRslt = parser.FindNode(L"//Language", currentNode);
	
	if (bRslt)
	{
		XmlNode newNode(parser.getDocument());
		newNode.SetName(L"Language");
		newNode.SetText(CATALAN_LANG_PROFILE);
		newNode.CreateElement();

		parser.ReplaceNode(newNode, currentNode);
		bRslt = parser.Save(file);
		g_log.Log(L"SkypeProfile::SetDefaultLanguage. Saved file '%s', result %u", (wchar_t *)file.c_str(), (wchar_t *) bRslt);
	}
	else
	{
		g_log.Log(L"SkypeProfile::SetDefaultLanguage. Could not find language node.");
	}
}
