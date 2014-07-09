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
 
#include "stdafx.h"
#include "Win32I18N.h"

LANGID Win32I18N::GetUserDefaultUILanguage(void)
{ 
	LANGID langid;
	langid = ::GetUserDefaultUILanguage();
	return PRIMARYLANGID(langid);
}

LANGID Win32I18N::GetSystemDefaultUILanguage(void)
{ 
	return ::GetSystemDefaultUILanguage();
}

BOOL CALLBACK Win32I18N::_enumUILanguagesProc(LPTSTR lpUILanguageString, LONG_PTR lParam)
{
	vector <LANGID>* langs = (vector <LANGID>*) lParam;
	LANGID langid;

	langid = (LANGID)wcstol(lpUILanguageString, NULL, 16);
	langs->push_back(langid);
	return TRUE;
}

vector <LANGID> Win32I18N::EnumUILanguages(void)
{
	vector <LANGID> langs;

	::EnumUILanguages(Win32I18N::_enumUILanguagesProc, MUI_LANGUAGE_ID, (LONG_PTR)&langs);
	return langs;
}
