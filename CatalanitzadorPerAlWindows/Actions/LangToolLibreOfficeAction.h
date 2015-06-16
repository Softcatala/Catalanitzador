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
#include "IOpenOffice.h"
#include "MultipleDownloads.h"
#include "ActionExecution.h"

class LangToolLibreOfficeAction : public Action, public ActionExecution
{
public:
		LangToolLibreOfficeAction(IRegistry* registry, IRunner* runner, IOpenOffice* libreOffice, IOpenOffice* apacheOpenOffice, DownloadManager* downloadManager);
		~LangToolLibreOfficeAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return LangToolLibreOfficeActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupOfficeAutomation;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion();
		virtual void CheckPrerequirements(Action * action);
		virtual ExecutionProcess GetExecutingProcess();
		virtual void FinishExecution(ExecutionProcess process) {};

protected:

		bool _shouldInstallJava();
		bool _doesJavaNeedsConfiguration();
		IOpenOffice* m_installingOffice;

private:

		bool _readJavaVersion(wstring& version);
		bool _isOpenOfficeInstalled(bool& bLibreInstalled, bool& bApacheInstalled);
		void _installJava();
	
		enum ExecutionStep
		{
			ExecutionStepNone,
			ExecutionStepInstallJava,
			ExecutionStepInstallExtension,
			ExecutionStepRetryInstallExtension,
			ExecutionStepFinished
		};
				
		IOpenOffice* m_libreOffice;
		IOpenOffice* m_apacheOpenOffice;
		IRegistry* m_registry;
		IRunner* m_runner;
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szFilenameJava[MAX_PATH];
		wstring m_version;
		ExecutionStep m_executionStep;
		MultipleDownloads m_multipleDownloads;
		
		bool m_shouldInstallJava;
		bool m_shouldConfigureJava;
};

