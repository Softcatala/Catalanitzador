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

#include "OpenOffice.h"

//
// Apache OpenOffice is a decendent of OpenOffice
// The first release was version 3.4.0, on 8 May 2012
//
class ApacheOpenOffice : public OpenOffice
{
public:

		ApacheOpenOffice(IRegistry* registry);

		virtual wchar_t * GetMachineRegistryKey() { return L"SOFTWARE\\OpenOffice\\OpenOffice"; }
		virtual wchar_t * GetUserDirectory() { return L"\\OpenOffice\\%u\\user\\"; }
};


