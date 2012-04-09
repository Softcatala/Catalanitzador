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
#include <stdio.h>

OSVersion::OSVersion()
{
	m_version = UnKnownOS;
}

OperatingVersion OSVersion::GetVersion()
{
	OSVERSIONINFOEX osvi;	
	BOOL bOsVersionInfoEx;

	if (m_version != UnKnownOS)
		return m_version;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

	if (bOsVersionInfoEx == FALSE || VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
		return UnKnownOS;

	// Documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/ms724832%28v=vs.85%29.aspx
	switch (osvi.dwMajorVersion)
	{
		case 5:
			m_version = _processXPAnd2000(osvi);			
			break;
		case 6:
			m_version = _processVistaAnd7(osvi);
			break;
		default:
			m_version = UnKnownOS;
			break;
	}

	return m_version;
}

bool OSVersion::IsWindows64Bits()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	HMODULE module = GetModuleHandle(_T("kernel32"));
	const char funcName[] = "IsWow64Process";
	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(module, funcName);

	if(fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),
                          &bIsWow64))
		return FALSE;
	}
	return bIsWow64 != FALSE;
}

void OSVersion::Serialize(ostream* stream)
{
	char szText[2048];
	char szAsciiName[2048];
	OSVERSIONINFOEX osvi;
	wchar_t* name;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*) &osvi);
	
	name = GetVersionText(GetVersion());
	WideCharToMultiByte(CP_ACP, 0, name, wcslen(name) + 1, szAsciiName, sizeof(szAsciiName), 0, 0);

	sprintf_s(szText, "\t<operating OSMajorVersion='%u' OSMinorVersion='%u' SPMajorVersion='%u' SPMinorVersion='%u' SuiteMask='%u' ProductType='%u' Bits='%u' Name='%s'/>\r\n",
		osvi.dwMajorVersion, 
		osvi.dwMinorVersion,
		osvi.wServicePackMajor,
		osvi.wServicePackMinor,
		osvi.wSuiteMask,
		osvi.wProductType,
		IsWindows64Bits() == true ? 64 : 32,
		szAsciiName);

	*stream << szText;
}

void OSVersion::GetLogInfo(wchar_t * szString, int size)
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*) &osvi);

	swprintf_s(szString, size / sizeof (wchar_t), L"OS Info. OS version %u.%u, SP version %u.%u, SuiteMask %u, ProductType %u, 64 bit %s, name %s",
		osvi.dwMajorVersion, 
		osvi.dwMinorVersion,
		osvi.wServicePackMajor,
		osvi.wServicePackMinor,
		osvi.wSuiteMask,
		osvi.wProductType,
		IsWindows64Bits() == true ? L"yes" : L"no",
		GetVersionText(GetVersion()));
}

 DWORD OSVersion::GetServicePackVersion()
 {	
	OSVERSIONINFOEX osvi;	
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*) &osvi);

	return MAKELONG(osvi.wServicePackMinor, osvi.wServicePackMajor);
 }

wchar_t* OSVersion::GetVersionText(OperatingVersion version)
{
	switch (version)
	{
		case UnKnownOS:
			return L"UnKnownOS";
		case WindowsXP:
			return L"WindowsXP";
		case WindowsVista:
			return L"WindowsVista";
		case Windows2000:
			return L"Windows2000";
		case Windows2008:
			return L"Windows2008";
		case Windows7:
			return L"Windows7";
		case Windows2008R2:
			return L"Windows2008R2";
		case WindowsXP64_2003:
			return L"WindowsXP64_2003";
		case Windows8:
			return L"Windows8";
		default:
			return L"Unknown enum";
	}	
}

OperatingVersion OSVersion::_processXPAnd2000(OSVERSIONINFOEX osvi)
{
	if (osvi.dwMinorVersion == 0)
	{
		return Windows2000;
	}

	if (osvi.dwMinorVersion == 1)
	{
		return WindowsXP;
	}

	if (osvi.dwMinorVersion == 2)
	{
		return WindowsXP64_2003;
	}

	return UnKnownOS;
}

OperatingVersion OSVersion::_processVistaAnd7(OSVERSIONINFOEX osvi)
{
	if (osvi.dwMinorVersion == 0)
	{
		if (osvi.wProductType == VER_NT_WORKSTATION)
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

	if (osvi.dwMinorVersion == 2)
	{
		return Windows8;
	}

	return UnKnownOS;
}

