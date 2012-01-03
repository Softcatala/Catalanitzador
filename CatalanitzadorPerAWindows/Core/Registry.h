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

class _APICALL Registry : public IRegistry
{
public:
		Registry();
		~Registry();

		bool OpenKey(HKEY hBaseKey, wchar_t* sSubKey, bool bWriteAccess);
		bool SetString(wchar_t* string, wchar_t* value);
		bool SetDWORD(wchar_t* string, DWORD value);
		bool GetString(wchar_t* sName, wchar_t* pBuffer, DWORD dwLength);
		bool GetDWORD(wchar_t* sName, DWORD *value);
		bool Close();
private:

		HKEY hKey;
};