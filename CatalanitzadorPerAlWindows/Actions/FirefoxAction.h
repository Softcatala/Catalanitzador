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
#include "FirefoxLangPackAction.h"
#include "FirefoxAcceptLanguagesAction.h"

using namespace std;

class FirefoxAction : public Action, public ActionExecution
{
public:
		FirefoxAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager);
		~FirefoxAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return NoAction;}
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool IsDownloadNeed();
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();		
		virtual void FinishExecution(ExecutionProcess process);
		virtual void CheckPrerequirements(Action * action);
		virtual ActionStatus GetStatus();
		virtual void Serialize(ostream* stream);
		virtual void SetStatus(ActionStatus value);
		
protected:

		wstring _getLocale();	
		wstring _getProfileRootDir();
		void _readVersionAndLocale();		
		bool _isAcceptLanguageOk();
		void _readInstallPath(wstring& path);

private:

		wstring _getVersionAndLocaleFromRegistry();
		void _extractLocaleAndVersion(wstring version);
		FirefoxLangPackAction * _getLangPackAction();
		FirefoxAcceptLanguagesAction * _getAcceptLanguagesAction();

		IRegistry* m_registry;
		IRunner* m_runner;
		wstring m_locale;
		wstring m_version;
		FirefoxLangPackAction* m_firefoxLangPackAction;
		FirefoxAcceptLanguagesAction* m_firefoxAcceptLanguagesAction;
		bool m_cachedVersionAndLocale;
		bool m_doFirefoxLangPackAction;
		bool m_doFirefoxAcceptLanguagesAction;
};
