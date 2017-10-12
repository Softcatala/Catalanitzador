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
#include "IRegistry.h"
#include "Firefox.h"
#include "FirefoxAddOn.h"


class LangToolFirefoxAction : public Action
{
public:
		LangToolFirefoxAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager);
		~LangToolFirefoxAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return LangToolFirefoxActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupBrowsers;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion();		
		virtual void CheckPrerequirements(Action * action);
		virtual const wchar_t* GetManualStep();

private:

		IRegistry* m_registry;
		IRunner* m_runner;
		wchar_t m_szFilename[MAX_PATH];
		wstring m_version;
		Firefox m_firefox;
		FirefoxAddOn m_firefoxAddOn;
		wstring m_manualSteps;
		OSVersion m_OSversion;
};

