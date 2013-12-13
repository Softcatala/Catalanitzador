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
#include "IRegistry.h"

#include "FirefoxAcceptLanguages.h"
#include "FirefoxMozillaServer.h"

using namespace std;

class FirefoxAcceptLanguagesAction : public Action
{
public:
		FirefoxAcceptLanguagesAction(wstring profileRootDir, wstring locale, wstring version);

		virtual wchar_t* GetName() {return L""; }
		virtual wchar_t* GetDescription() {return L""; }
		virtual ActionID GetID() const { return FirefoxActionID;}
		virtual ActionGroup GetGroup() const {return ActionGroupBrowsers;}
		virtual bool IsDownloadNeed() { return false;}		
		virtual bool IsNeed();
		virtual void Execute();
		virtual void CheckPrerequirements(Action * action);
		virtual bool IsVisible() {return false; }
		virtual const wchar_t* GetVersion() {return m_version.c_str();}
		
protected:	
		
		bool _isAcceptLanguageOk();

private:

		FirefoxAcceptLanguages m_acceptLanguages;
		wstring m_version;
};
