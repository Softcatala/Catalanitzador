/* 
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "IRunner.h"
#include "FirefoxMozillaServer.h"

using namespace std;

class FirefoxLangPackAction : public Action
{
public:
		FirefoxLangPackAction(IRunner* runner, wstring installPath, wstring locale, wstring version, DownloadManager* downloadManager);
		~FirefoxLangPackAction();

		virtual wchar_t* GetName() {return L""; }
		virtual wchar_t* GetDescription() {return L""; }
		virtual ActionID GetID() const { return FirefoxLangPackActionID;}
		virtual ActionGroup GetGroup() const {return ActionGroupBrowsers;}
		virtual bool IsDownloadNeed();
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();				
		virtual void CheckPrerequirements(Action * action);
		virtual bool IsVisible() {return false; }
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion() {return m_version.c_str();}
		void SetLocaleAndUpdateStatus(wstring locale);

private:

		bool _isSupportedChannel();
		wstring _getVersionAndLocaleFromRegistry();
		bool _isLocaleInstalled();
		void _extractLocaleAndVersion(wstring version);
		FirefoxMozillaServer * _getMozillaServer();
		
		IRunner* m_runner;
		wstring m_locale;
		wstring m_version;
		wstring m_installPath;		
		FirefoxMozillaServer* m_mozillaServer;
		wchar_t m_szFilename[MAX_PATH];
};
