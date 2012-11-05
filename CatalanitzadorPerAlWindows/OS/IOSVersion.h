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

#include <windows.h>
#include <ostream>

using namespace std;

// Keep in sync with OSVersion::GetVersionText
enum OperatingVersion
{
	UnKnownOS,
	WindowsXP,
	WindowsVista,
	Windows2000,
	Windows2008,
	Windows7,
	Windows2008R2,
	WindowsXP64_2003,
	Windows8
};

class IOSVersion
{
public:
		virtual OperatingVersion GetVersion() = 0;
		virtual wchar_t* GetVersionText(OperatingVersion version) = 0;
		virtual bool IsWindows64Bits() = 0;
		virtual void GetLogInfo(wchar_t * szString, int size) = 0;
		virtual void Serialize(ostream* stream) = 0;
		virtual DWORD GetServicePackVersion() = 0;
};