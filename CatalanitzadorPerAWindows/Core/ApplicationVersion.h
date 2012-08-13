/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include <string>
#include <vector>

using namespace std;

class _APICALL ApplicationVersion
{
	public:
			ApplicationVersion() {}
			ApplicationVersion(wstring version) { m_version = version;}
			vector <wstring> GetComponents();

			bool operator == (ApplicationVersion other);
			bool operator != (ApplicationVersion other);
			bool operator > (ApplicationVersion other);
			bool operator < (ApplicationVersion other);
			bool operator >= (ApplicationVersion other);
			bool operator <= (ApplicationVersion other);

			wstring& GetString() {return m_version;}

	private:

			vector <wstring> GetComponents(wstring version);
			
			wstring m_version;
			vector <wstring> m_components;
};
