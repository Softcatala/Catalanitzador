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
#include "Runner.h"
#include "ActionExecution.h"
#include "FirefoxAcceptLanguages.h"
#include "FirefoxMozillaServer.h"

using namespace std;

class FirefoxAction : public Action, public ActionExecution
{
public:
		FirefoxAction(IRegistry* registry, IRunner* runner);
		~FirefoxAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return Firefox;}
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool IsDownloadNeed();
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual void FinishExecution(ExecutionProcess process);
		virtual void CheckPrerequirements(Action * action);
		virtual ActionStatus GetStatus();
		
protected:
	
		wstring _getProfileRootDir();
		bool _readVersionAndLocale();
		wstring _getLocale() {return m_locale;}
		bool _isAcceptLanguageOk();

private:
		bool _isLocaleInstalled();
		void _extractLocaleAndVersion(wstring version);
		FirefoxAcceptLanguages * _getAcceptLanguages();
		FirefoxMozillaServer * _getMozillaServer();

		IRegistry* m_registry;
		IRunner* m_runner;
		wstring m_locale;
		wstring m_version;
		FirefoxAcceptLanguages* m_acceptLanguages;
		FirefoxMozillaServer* m_mozillaServer;
		wchar_t m_szFilename[MAX_PATH];
};
