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

#include "stdafx.h"
#include "LibreOffice.h"
#include "ApplicationVersion.h"

LibreOffice::LibreOffice(IRegistry* registry) : OpenOffice(registry)
{

}

wstring LibreOffice::_getPreferencesDirectory()
{
	wchar_t directory[1024];
	wstring location;
	int major, subdir_num;
	ApplicationVersion applicationVersion(GetVersion());
	
	location = _getAppDataDir();
	major = applicationVersion.GetMajorVersion();

	if (major == 5)
	{
		// Version 5.00 identifies internally as 4.5 and used /4/ subdirectory
		subdir_num = 4;
	}
	else
	{
		// This is based on the assumption that directory changes with every major version happened in versions 3 and 4
		subdir_num = applicationVersion.GetMajorVersion();
	}
	
	swprintf_s(directory, GetUserDirectory(), subdir_num);
	location += directory;
	return location;
}
