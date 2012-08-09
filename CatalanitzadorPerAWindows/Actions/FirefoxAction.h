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

#include "Action.h"
#include "Runner.h"
#include "OSVersion.h"
#include "IRegistry.h"
#include "IOSVersion.h"

#include <vector>
#include <algorithm>

using namespace std;

class _APICALL FirefoxAction : public Action
{
public:
		FirefoxAction (IRegistry* registry);

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return Firefox;}
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual bool IsExecuting();
		virtual void FinishExecution();

protected:
		virtual void _getProfileRootDir(wstring &location);
		bool _readVersionAndLocale();

		wstring* _getLocale() {return &m_locale;}
		vector <wstring> * _getLanguages() {return &m_languages;}

private:
		
		bool _readLanguageCode();
		void _createPrefsString(wstring& string);
		void _addCatalanToArrayAndRemoveOldIfExists();
		void _getFirstLanguage(wstring& regvalue);
		void _writeLanguageCode(wstring &langcode);
		void _getPrefLine(wstring langcode, wstring& line);
		void _addFireForLocale();
		void _parseLanguage(wstring regvalue);

		void _getProfilesIniLocation(wstring &location);
		bool _getPreferencesFile(wstring &location);
		bool _getProfileLocationFromProfilesIni(wstring file, wstring &profileLocation);
		vector <DWORD> _getProcessIDs();

		IRegistry* m_registry;
		vector <wstring> m_languages;
		wstring m_locale;
		wstring m_version;
		bool m_CachedLanguageCode;
};

