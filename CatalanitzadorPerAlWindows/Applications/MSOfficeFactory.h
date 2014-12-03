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

#include "MSOffice.h"
#include "IOSVersion.h"

// Static factory class that determines which versions of MS Office are installed and
// returns a list of MSOffice objects to manage the different versions
class MSOfficeFactory
{
public:

		static vector <MSOffice> GetInstalledOfficeInstances(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner);
		// These should be private but it is not possible to change member visibility with using on static members
		static bool _isVersionInstalled(IOSVersion* OSVersion, IRegistry* registry, MSOfficeVersion version);
		static void _removeOffice2003IfNewerIsPresent(vector <MSOffice>& instances);
};

