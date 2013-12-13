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
#include "TriBool.h"
#include "ActionExecution.h"
#include "ChromeProfile.h"


using namespace std;

#define CHROME_REGISTRY_PATH L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome"

class ChromeAction : public Action, public ActionExecution
{
public:
		ChromeAction(IRegistry* registry);
		~ChromeAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return ChromeActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupBrowsers;}
		virtual bool IsDownloadNeed() {return false;}
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual void FinishExecution(ExecutionProcess process);
		virtual void CheckPrerequirements(Action * action);
		void SetChromeProfile(ChromeProfile *profile);
		
protected:
		void _readVersion();
		bool _isInstalled();
		bool _readLanguageCode(wstring& langcode);

private:		
		void _readInstallLocation(wstring& path);
		bool _findUserInstallation(wstring &);
		bool _findSystemInstallation(wstring &);
		wstring _getProfileRootDir();

		ChromeProfile* m_chromeProfile;
		bool m_allocatedProfile;

		wstring m_version;
		IRegistry* m_registry;
		TriBool m_isInstalled;
		ActionStatus uiStatus;
};

