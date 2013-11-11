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

enum ActionID
{
	// Do not modify or assign different meanings to these since they are used in the server side to identify 
	// actions in a unique manner and we have data from previous sessions with already existing values
	NoAction = 0,
	WindowsLPIActionID = 1,
	MSOfficeLPIActionID = 2,
	ConfigureLocaleActionID = 3,
	IEAcceptLanguageActionID = 4,
	IELPIActionID = 5,
	ConfigureDefaultLanguageActionID = 6,
	ChromeActionID = 7,
	FirefoxActionID = 8,
	OpenOfficeActionID = 9,
	AcrobatReaderActionID = 10,
	WindowsLiveActionID = 11,
	CatalanitzadorUpdateActionID = 12,
	SkypeActionID = 13,
	iTunesActionID = 14,
    MacSystemActionActionID = 15,
    MacSpellCheckerActionID = 16,
	FirefoxLangPackActionID = 17,
	LangToolLibreOfficeActionID = 18
};