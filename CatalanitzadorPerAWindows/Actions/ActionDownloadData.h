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


#define MSOFFICEACTION_2010 L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2010/LanguageInterfacePack-x86-ca-es.exe"
#define MSOFFICEACTION_2007 L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2007/office2007-lip.exe"
#define MSOFFICEACTION_2003 L"http://www.softcatala.org/pub/softcatala/catalanitzador/office2003/office2003-lip.cab"
#define MSOFFICEACTION_OUTLOOK_CONNECTOR L"http://www.softcatala.org/pub/softcatala/catalanitzador/OutlookHotmailConnector/OutlookConnector-cat.exe"

#define IELPI_IE7 L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/ie7lip-x86-CAT.exe"
#define IELPI_IE8_XP L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE8-WindowsXP-x86-CAT.exe"
#define IELPI_IE8_VISTA L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE8-Windows6.0-LanguagePack-x86-CAT.msu"
#define IELPI_IE9_VISTA L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.0-LanguagePack-x86-cat.msu"
#define IELPI_IE9_7 L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.1-LanguagePack-x86-cat.msu"
#define IELPI_IE9_7_64BITS L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.1-LanguagePack-x64-cat.msu"

#define ADOBEREADER_95 L"http://www.softcatala.org/pub/softcatala/catalanitzador/AdobeReader/AdbeRdr950_ca_ES.exe"
#define ADOBEREADER_1010 L"http://www.softcatala.org/pub/softcatala/catalanitzador/AdobeReader/AdbeRdr1010_ca_ES.exe"

#define MSLIVE2009 L"http://www.softcatala.org/pub/softcatala/catalanitzador/MicrosoftLive/wlsetup-all-2009-ca-14.0.8117.416.exe"

enum DownloadID
{
	DI_MSOFFICEACTION_2010,
	DI_MSOFFICEACTION_2007,
	DI_MSOFFICEACTION_2003,
	DI_MSOFFICEACTION_OUTLOOK_CONNECTOR,	
	DI_IELPI_IE7,
	DI_IELPI_IE8_XP,
	DI_IELPI_IE8_VISTA,
	DI_IELPI_IE9_VISTA,
	DI_IELPI_IE9_7,
	DI_IELPI_IE9_7_64BITS,
	DI_ADOBEREADER_95,
	DI_ADOBEREADER_1010,
	DI_MSLIVE2009,
	DI_UNKNOWN,
	DI_LENGTH,
};

