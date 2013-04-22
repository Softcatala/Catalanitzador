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
#include "IRunner.h"
#include "IRegistry.h"
#include "IFileVersionInfo.h"

class WindowsLiveAction : public Action
{
public:
		WindowsLiveAction(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo, DownloadManager* downloadManager);
		~WindowsLiveAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return WindowsLiveActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual void CheckPrerequirements(Action * action);
		virtual const wchar_t* GetVersion();

		bool IsPreRebootRequired();

protected:
		
		int _getMajorVersion();
		bool _isLangSelected();
		bool _isDownloadAvailable();
		
private:
		
		wstring _getMostRecentWLSetupLogFile();
		void _dumpWLSetupErrors();
		void _getInstallerLocation(wstring& location);		
		bool _isLangSelected2011();
		bool _isLangSelected2009();
		void _readVersionInstalled();

		IRunner* m_runner;
		IRegistry* m_registry;
		IFileVersionInfo* m_fileVersionInfo;

		wstring m_version;
		wchar_t m_szFilename[MAX_PATH];
};

