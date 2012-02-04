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
 
#pragma once

#include <WinInet.h>
#include <string>
using namespace std;

class Url
{
public:
		Url(wchar_t* url);
		Url(wstring string);
		wchar_t * GetFileName() {return m_filename;}
		wchar_t * GetPathAndFileName() {return m_path;}
		wchar_t * GetHostname() {return m_hostname;}
		static void EncodeParameter(wstring parameter, wstring& encoded);

private:
		void _extractfilename(wchar_t* path);
		void _extractpath(wchar_t* url);

		URL_COMPONENTS m_components;		
		wchar_t m_filename[1024];
		wchar_t m_hostname[1024];
		wchar_t m_path[1024];
};