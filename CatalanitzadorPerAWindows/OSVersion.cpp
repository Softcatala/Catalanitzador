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

#include "stdafx.h"
#include "OSVersion.h"

OperatingVersion OSVersion::m_version = UnKnownOS;

OperatingVersion OSVersion::GetVersion ()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	BOOL bOsVersionInfoEx;

	if (m_version != UnKnownOS)
		return m_version;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

	if (bOsVersionInfoEx == NULL || VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
		return UnKnownOS;

	switch (osvi.dwMajorVersion)
	{
		case 5:
			m_version = _processXPAnd2000 (osvi);			
			break;
		case 6:
			m_version = _processVistaAnd7 (osvi);
			break;
		default:
			m_version = UnKnownOS;
			break;
	}

	return m_version;
}

OperatingVersion OSVersion::_processXPAnd2000 (OSVERSIONINFOEX osvi)
{
	if (osvi.dwMinorVersion == 0)
	{
		return WindowsXP;
	}

	if (osvi.dwMinorVersion == 1)
	{
		return Windows2000;
	}
	return UnKnownOS;
}

OperatingVersion OSVersion::_processVistaAnd7 (OSVERSIONINFOEX osvi)
{
	if (osvi.dwMinorVersion == 0)
	{
		if (osvi.wProductType == VER_NT_WORKSTATION )
			return WindowsVista;                
		else 
			return Windows2008;
	}

	if (osvi.dwMinorVersion == 1)
	{
		if (osvi.wProductType == VER_NT_WORKSTATION)
			return Windows7;
		else
		return Windows2008R2;                
	}
	return UnKnownOS;
}

