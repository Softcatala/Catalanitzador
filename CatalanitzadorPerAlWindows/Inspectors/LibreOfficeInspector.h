/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "Inspector.h"
#include "IRegistry.h"
#include "XmlParser.h"

class LibreOfficeInspector : public Inspector
{
public:
		LibreOfficeInspector(IRegistry* registry);

		virtual InspectorID GetID() const {return LibreOfficeInspectorID;}
		virtual void Execute();		
		
private:
		
		void _readVersionInstalled();
		void _readLanguage();
		void _getUIFilesInstalled();
		void _getDictInstalled();
		void _getPreferencesFile(wstring& location);
		void _readLocale(wstring &locale);
		static bool _readNodeCallback(XmlNode node, void *data);

		IRegistry* m_registry;
		wstring m_version;
};
