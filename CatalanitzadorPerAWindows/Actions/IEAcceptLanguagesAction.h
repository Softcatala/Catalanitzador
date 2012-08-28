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

#include "Action.h"
#include "IRegistry.h"
#include "InternetExplorerVersion.h"

#include <vector>
#include <algorithm>

using namespace std;

class IEAcceptLanguagesAction : public Action
{
public:
		
		IEAcceptLanguagesAction(IRegistry* registry);

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return IEAcceptLanguage;};
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual void CheckPrerequirements(Action * action);

protected:

		void _parseLanguage(wstring regvalue);
		void _createRegistryString(wstring &regvalue);
		void _addCatalanToArrayAndRemoveOldIfExists();
		vector <wstring> * _getLanguages() {return &m_languages;}
private:
		
		bool _isCurrentLanguageOk(wstring& firstlang);
		void _getCurrentLanguage(wstring& lang);
		void _getFirstLanguage(wstring& regvalue);
		void _readLanguageCode(wstring& langcode);
		bool _writeLanguageCode(wstring langcode);
		void _createRegistryStringTwoLangs(wstring &regvalue, float average);	
		
		IRegistry* m_registry;
		vector <wstring> m_languages;
		wstring m_version;
		InternetExplorerVersion m_explorerVersion;
};

