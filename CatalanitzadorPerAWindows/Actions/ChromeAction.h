/* 
 * Copyright (C) 2012 Xavier Ivars-Ribes <xavi.ivars@gmail.com>
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

#include <vector>
#include <algorithm>

using namespace std;

class _APICALL ChromeAction : public Action
{
public:
		
		ChromeAction(IRegistry* registry);

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual int GetID() { return Chrome;};
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual void CheckPrerequirements(Action * action);
		virtual bool IsExecuting();
		virtual void FinishExecution();

		void ParseLanguage(wstring regvalue);
		void CreateJSONString(wstring &regvalue);
		void AddCatalanToArrayAndRemoveOldIfExists();
		vector <wstring> * GetLanguages() {return &m_languages;}

private:
		
		void _getFirstLanguage(wstring& regvalue);
		void _readVersion();
		void _readInstallLocation(wstring& path);
		bool _readLanguageCode(wstring& langcode);
		bool _writeLanguageCode(wstring langcode);
		void _createRegistryStringTwoLangs(wstring &regvalue, float average);
		bool _isChromeAppLocaleOk();

		bool _findIntl(wstring,int&);
		bool _findSemicolon(wstring,int&);
		bool _findStartBlock(wstring,int&);
		bool _findAcceptedKey(wstring,int&);
		bool _findAcceptedValue(wstring,int&);
		bool _findLanguageString(wstring,int &,wstring &);
		bool _findAppLocaleKey(wstring line, int & pos);
		DWORD _getProcessID();

		wstring m_version;
		IRegistry* m_registry;
		vector <wstring> m_languages;
		bool isInstalled;
};

