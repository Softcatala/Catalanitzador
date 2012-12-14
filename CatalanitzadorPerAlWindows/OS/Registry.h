/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "IRegistry.h"

class Registry : public IRegistry
{
public:
		Registry();
		~Registry();

		virtual bool OpenKey(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess);
		virtual bool OpenKeyNoWOWRedirect(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess);
		virtual bool CreateKey(HKEY hBaseKey, wchar_t* sSubKey);
		virtual bool SetString(wchar_t* string, wchar_t* value);
		virtual bool SetMultiString(wchar_t* string, wchar_t* value);
		virtual bool SetDWORD(wchar_t* string, DWORD value);
		virtual bool GetString(wchar_t* sName, wchar_t* pBuffer, DWORD dwLength);
		virtual bool GetDWORD(wchar_t* sName, DWORD *value);
		virtual bool Close();
		virtual bool RegEnumKey(DWORD dwIndex, wstring& key);
		virtual bool RegEnumValue(DWORD dwIndex, wstring& value);

private:

		HKEY hKey;
};