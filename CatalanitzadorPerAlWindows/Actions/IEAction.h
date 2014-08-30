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
#include "IOSVersion.h"
#include "InternetExplorerVersion.h"
#include "IFileVersionInfo.h"
#include "IELPIAction.h"
#include "IEAcceptLanguagesAction.h"

using namespace std;

class IEAction : public Action
{
public:
		IEAction(IELPIAction* LPIAction, IEAcceptLanguagesAction* acceptLanguagesAction);

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return NoAction;}
		virtual ActionGroup GetGroup() const {return ActionGroupBrowsers;}
		virtual bool IsDownloadNeed();
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();		
		virtual void CheckPrerequirements(Action * action);
		virtual ActionStatus GetStatus();
		virtual void Serialize(ostream* stream);
		virtual void SetStatus(ActionStatus value);
		virtual LPCWSTR GetLicenseID();
		virtual ActionID DependsOn() const { return WindowsLPIActionID;};
		
private:

		IELPIAction * _getLPIAction();
		IEAcceptLanguagesAction * _getAcceptLanguagesAction();

		IRegistry* m_registry;
		IRunner* m_runner;
		IOSVersion* m_OSVersion;
		wstring m_locale;
		wstring m_version;
		IFileVersionInfo* m_fileVersionInfo;
		IELPIAction * m_LPIAction;
		IEAcceptLanguagesAction* m_acceptLanguagesAction;
		bool m_doAcceptLanguagesAction;
		bool m_doLPIAction;
};
