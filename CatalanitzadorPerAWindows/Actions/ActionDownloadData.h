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

#define WINDOWSLPIACTION_XP L"http://www.softcatala.org/pub/softcatala/catalanitzador/WindowsXP/Build2006Professional/LIPsetup.msi"
#define WINDOWSLPIACTION_XP_SP2 L"http://www.softcatala.org/pub/softcatala/catalanitzador/WindowsXP/XP2orlater/LIPsetup.msi"
#define WINDOWSLPIACTION_VISTA L"http://www.softcatala.org/pub/softcatala/catalanitzador/WindowsVista/lip_ca-es.mlc"
#define WINDOWSLPIACTION_7 L"http://www.softcatala.org/pub/softcatala/catalanitzador/Windows7/LIP_ca-ES-32bit.mlc"
#define WINDOWSLPIACTION_7_64BITS L"http://www.softcatala.org/pub/softcatala/catalanitzador/Windows7/LIP_ca-ES-64bit.mlc"

#define IELPI_IE7 L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/ie7lip-x86-CAT.exe"
#define IELPI_IE8_XP L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE8-WindowsXP-x86-CAT.exe"
#define IELPI_IE8_VISTA L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE8-Windows6.0-LanguagePack-x86-CAT.msu"
#define IELPI_IE9_VISTA L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.0-LanguagePack-x86-cat.msu"
#define IELPI_IE9_7 L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.1-LanguagePack-x86-cat.msu"
#define IELPI_IE9_7_64BITS L"http://www.softcatala.org/pub/softcatala/catalanitzador/InternetExplorer/IE9-Windows6.1-LanguagePack-x64-cat.msu"

#define OPENOFFICE_33 L"http://www.softcatala.org//pub/softcatala/catalanitzador/OpenOffice.org/openoffice33-langpack.cab"

enum DownloadID
{
	DI_MSOFFICEACTION_2010,
	DI_MSOFFICEACTION_2007,
	DI_MSOFFICEACTION_2003,
	DI_MSOFFICEACTION_OUTLOOK_CONNECTOR,
	DI_WINDOWSLPIACTION_XP,
	DI_WINDOWSLPIACTION_XP_SP2,
	DI_WINDOWSLPIACTION_VISTA,
	DI_WINDOWSLPIACTION_7,
	DI_WINDOWSLPIACTION_7_64BITS,
	DI_IELPI_IE7,
	DI_IELPI_IE8_XP,
	DI_IELPI_IE8_VISTA,
	DI_IELPI_IE9_VISTA,
	DI_IELPI_IE9_7,
	DI_IELPI_IE9_7_64BITS,
	DI_OPENOFFICE_33,
	DI_UNKNOWN,
	DI_LENGTH
};

