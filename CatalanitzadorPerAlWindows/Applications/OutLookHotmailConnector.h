/* 
 * Copyright (C) 2012-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "IRegistry.h"
#include "IRunner.h"
#include "ConfigurationFileActionDownload.h"

//
// When we launched version 1.0 users were complaing the Outlook will not start any more
// If you install MsOffice LanguagePack, if the user has the OutlookHotmail connector, you
// need also to install the Catalan version.
//
class OutLookHotmailConnector
{
public:	
		OutLookHotmailConnector(bool MSOffice2013OrHigher, IRegistry *registry);
		~OutLookHotmailConnector();

		bool IsNeed();
		bool Execute(IRunner* runner);
		void GetDownloadConfigurationAndTempFile(ConfigurationFileActionDownload& downloadVersion, wstring& tempFile);

protected:
		virtual wstring _getOutLookPathToCatalanLibrary();

private:
		
		bool m_MSOffice2010OrPrevious;
		wstring m_installerLocation;
		IRegistry *m_registry;
};

