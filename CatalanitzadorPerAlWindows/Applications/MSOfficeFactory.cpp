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
#include "MSOfficeFactory.h"

struct 
{
	MSOfficeVersion version;
	const wchar_t* regkey;
	bool is64bits;

} installedVersions[] = 
{
	{ MSOffice2013,		L"15.0", false},
	{ MSOffice2013_64,	L"15.0", true},
	{ MSOffice2010,		L"14.0", false},
	{ MSOffice2010_64,	L"14.0", true},
	{ MSOffice2007,		L"12.0", false},	
	{ MSOffice2003,		L"11.0", false}
};

bool MSOfficeFactory::_isVersionInstalled(IOSVersion* OSVersion,IRegistry* registry, MSOfficeVersion version)
{
	wchar_t szValue[1024], szKey[1024];	
	bool isInstalled = false;
	int index = -1;

	for (int i = 0; i < sizeof(installedVersions) / sizeof(installedVersions[0]); i++)
	{
		if (installedVersions[i].version == version)
		{
			index = i;
			break;
		}
	}

	// On Window 32 OpenKeyNoWOWRedirect calls return the same than OpenKey
	// In 32 bits, we know that you cannot install Office 64 bits
	if (installedVersions[index].is64bits && OSVersion->IsWindows64Bits() == false)
		return false;
	
	swprintf_s(szKey, L"SOFTWARE\\Microsoft\\Office\\%s\\Common\\InstallRoot", installedVersions[index].regkey);
	if (installedVersions[index].is64bits ? registry->OpenKeyNoWOWRedirect(HKEY_LOCAL_MACHINE, szKey, false) :
		registry->OpenKey(HKEY_LOCAL_MACHINE, szKey, false))
	{	
		if (registry->GetString(L"Path", szValue, sizeof (szValue)))
		{
			if (wcslen(szValue) > 0)
				isInstalled = true;
		}
		registry->Close();
	}
	return isInstalled;
}

vector <MSOffice> MSOfficeFactory::GetInstalledOfficeInstances(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner)
{
	vector <MSOffice> instances;
	
	for (int i = 0; i < sizeof(installedVersions) / sizeof(installedVersions[0]); i++)
	{
		if (_isVersionInstalled(OSVersion, registry, installedVersions[i].version))
		{
			MSOffice instance(OSVersion, registry, win32I18N, runner, installedVersions[i].version);
			instances.push_back(instance);
		}
	}
	
	return instances;
}

