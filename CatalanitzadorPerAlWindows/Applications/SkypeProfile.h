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

#include "Action.h"
#include "XmlParser.h"
#include "Tribool.h"

class SkypeProfile
{
public:	

		SkypeProfile();

		TriBool CanReadDefaultProfile();
		bool IsDefaultAccountLanguage();
		void SetDefaultLanguage();

protected:
		
		bool _isDefaultInstallLanguage();		
		bool _readDefaultAccountLanguage(wstring& language);
		bool _readDefaultAccount(wstring& account);

private:
	
		void _getSharedFile(wstring& location);
		void _getConfigFile(wstring& location, const wstring account);
		static bool _readDefaultAccountNodeCallback(XmlNode node, void *data);
		static bool _readDefaultAccountLanguageNodeCallback(XmlNode node, void *data);

		TriBool m_canReadDefaultProfile;
		
};

