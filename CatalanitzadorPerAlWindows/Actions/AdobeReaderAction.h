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
#include "HideApplicationWindow.h"
#include "ActionExecution.h"

class AdobeReaderAction : public Action, public ActionExecution
{
public:
		AdobeReaderAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager);
		~AdobeReaderAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return AcrobatReaderActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupOfficeAutomation;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion() {return m_version.c_str();}
		virtual void CheckPrerequirements(Action * action);
		virtual void FinishExecution(ExecutionProcess process);

protected:

		void _enumVersions(vector <wstring>& versions);
		void _readInstalledLang(wstring version);
		int _getMajorVersion();		

		wstring m_lang;

private:

		enum ExecutionStep
		{
			ExecutionStepNone,
			ExecutionStep1,
			ExecutionStep2	
		};
		
		bool _isLangPackInstalled();
		void _readUninstallGUID();
		void _uninstall();
		void _installVersion();
		void _readVersionInstalled();
		void _initProcessNames();
		void _dumpInstallerErrors();

		wstring m_version;
		wstring m_GUID;
		ExecutionStep m_executionStep;
		IRunner* m_runner;
		IRegistry* m_registry;
		HideApplicationWindow m_hideApplicationWindow;
		
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szTempPath[MAX_PATH];
};

