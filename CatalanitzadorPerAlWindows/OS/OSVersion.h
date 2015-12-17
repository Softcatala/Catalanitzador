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

#include <ostream>
#include "IOSVersion.h"

using namespace std;


class OSVersion : public IOSVersion
{
public:
		OSVersion();
		virtual OperatingVersion GetVersion();
		virtual wchar_t* GetVersionText(OperatingVersion version);
		virtual bool IsWindows64Bits();
		virtual void GetLogInfo(wchar_t * szString, int size);
		virtual void Serialize(ostream* stream);
		virtual DWORD GetServicePackVersion();
		virtual DWORD GetBuildNumber();

private:
		static OperatingVersion _processVistaAnd7(OSVERSIONINFOEX osvi);
		static OperatingVersion _processXPAnd2000(OSVERSIONINFOEX osvi);
		static OperatingVersion _process10(OSVERSIONINFOEX osvi);
		OperatingVersion m_version;
};