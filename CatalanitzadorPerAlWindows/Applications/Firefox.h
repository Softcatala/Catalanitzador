/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "IOSVersion.h"
#include "IRegistry.h"

class Firefox
{
public:

		Firefox(IRegistry* registry, IOSVersion* OSVersion);

		wstring GetVersion(); 
		wstring GetLocale();
		wstring GetInstallPath();
		void InvalidateCache() {m_cachedValues = false; }
		wstring GetUserDataDirectory();
		wstring GetProfileRootDir();
		bool Is64Bits() { return m_is64bits;}
		
protected:

		void _readInstallPath();
		wstring _getVersionAndLocaleFromRegistry();
		void _readVersionAndLocale();
		void _extractLocaleAndVersion(wstring versionWithLocale);
private:

		wstring m_version;
		wstring m_locale;
		wstring m_installPath;
		bool m_cachedValues;
		bool m_is64bits;
		IRegistry* m_registry;
		IOSVersion* m_OSVersion;
};

