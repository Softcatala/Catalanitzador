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

#pragma once

#include "IRunner.h"
#include "IOSVersion.h"
#include "IRegistry.h"
#include "TriBool.h"

enum MSOfficeVersion
{
	MSOfficeUnKnown,
	NoMSOffice,
	MSOffice2003,
	MSOffice2007,
	MSOffice2010,
	MSOffice2010_64,
	MSOffice2013,
	MSOffice2013_64
};

struct RegKeyVersion
{
	wchar_t* VersionNumber;
	wchar_t* InstalledLangMapKey;
	bool InstalledLangMapKeyIsDWord;
};


class MSOffice
{
public:
		MSOffice(IRegistry* registry, IRunner* runner);

		MSOfficeVersion _readVersionInstalled();
		void _readIsLangPackInstalled(MSOfficeVersion version, TriBool& langPackInstalled, bool& langPackInstalled64bits);
		bool _isLangPackForVersionInstalled(MSOfficeVersion version, bool b64bits);
		void _setDefaultLanguage(MSOfficeVersion officeVersion);
		bool _isDefaultLanguage(MSOfficeVersion officeVersion);
		
protected:
		

private:

		RegKeyVersion _getRegKeys(MSOfficeVersion officeVersion);
		void _readDefaultLanguage(MSOfficeVersion officeVersion, bool& isCatalanSetAsDefaultLanguage, bool& followSystemUIOff);
		bool _isVersionInstalled(RegKeyVersion regkeys, bool b64bits);

		IRunner* m_runner;
		IRegistry* m_registry;		
};


