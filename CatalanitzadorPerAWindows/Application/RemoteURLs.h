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

#include "version.h"

#define DEVELOPMENT_VERSION 1

#if DEVELOPMENT_VERSION

	#define APPLICATON_WEBSITE	L"http://dev-catalanitzador.softcatala.org"
	#define UPLOAD_URL L"http://dev-catalanitzador.softcatala.org/parser.php"
	#define REMOTE_CONFIGURATION_URL L"http://www.softcatala.org/pub/beta/catalanitzador/configuration.xml"

#else

	#define APPLICATON_WEBSITE	L"http://catalanitzador.softcatala.org"
	#define UPLOAD_URL L"http://catalanitzador.softcatala.org/parser.php"
	#define REMOTE_CONFIGURATION_URL L"http://www.softcatala.org/pub/catalanitzador/configuration.xml"

#endif
