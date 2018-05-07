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

#include "PlatformDefinitions.h"

#pragma once

#define APP_MAJOR_VERSION		2
#define APP_MINOR_VERSION		8
#define APP_REVISION			2

#define STRING_VERSION				_STR("2.8.2")
#define STRING_VERSION_RESOURCES	"2.8.2.0"

#define DEVELOPMENT_VERSION 1 // Comment for release version

#if (APP_REVISION % 2 == 0) 
	#ifdef DEVELOPMENT_VERSION 
		#error "Development versions should have an odd revision number"
	#endif
#endif

#if (APP_REVISION % 2 > 0) 
	#ifndef DEVELOPMENT_VERSION 
		#error "Odd versions should be development versions"
	#endif
#endif

   

  