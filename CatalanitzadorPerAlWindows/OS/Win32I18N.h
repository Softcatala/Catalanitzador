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

#include "IWin32I18N.h"
#include <windows.h>
#include <vector>
using namespace std;

class Win32I18N : public IWin32I18N
{
public:
		virtual LANGID GetUserDefaultUILanguage(void);
		virtual LANGID GetSystemDefaultUILanguage(void);		
		virtual vector <LANGID> EnumUILanguages(void);

private:
		static BOOL CALLBACK _enumUILanguagesProc(LPTSTR lpUILanguageString, LONG_PTR lParam);
};

