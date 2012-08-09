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

#include "XmlParser.h"
#include "Configuration.h"
#include <string>

using namespace std;

class _APICALL ConfigurationRemoteXmlParser
{
	public:
			ConfigurationRemoteXmlParser(wstring file);
			void Parse();
			ConfigurationRemote& GetConfiguration() {return m_configuration;}

	private:
		
			enum ConfigurationBlock
			{
				ConfigurationBlockNone,
				ConfigurationBlockCompatibility,
				ConfigurationBlockActions,
				ConfigurationBlockAction
			};

			void ParseNode(XmlNode node);
			void ParseBlockCompatibility(XmlNode node);
			void ParseBlockActions(XmlNode node);
			void ParseBlockAction(XmlNode node);

			static bool _readNodeCallback(XmlNode node, void *data);
			
			ConfigurationBlock m_configurationBlock;
			wstring m_file;
			ConfigurationRemote m_configuration;
			ConfigurationFileActionDownloads* m_pFileActionDownloads;
			ConfigurationFileActionDownload* m_pFileActionDownload;
};
