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
#include "IWin32I18N.h"
#include "IOSVersion.h"
#include "WindowsLPIBaseAction.h"
#include <algorithm>

class Windows8LPIAction : public WindowsLPIBaseAction
{
public:
		Windows8LPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner, DownloadManager* downloadManager);
		~Windows8LPIAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return WindowsLPIActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupWindows;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual bool IsRebootNeed() const;
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual void CheckPrerequirements(Action * action);
		virtual LPCWSTR GetLicenseID();
		virtual bool IsDownloadNeed();
		
protected:

		bool _isLangPackInstalled();
		wchar_t* _getDownloadID();
		void _setLanguagePanel();
		const wstring _getScriptFile() {return m_scriptfile;}
		bool _isASupportedSystemLanguage();
		bool _isLanguagePanelFirstForLanguage(wstring langcode);
		
private:

		enum ExecutionStep
		{
			ExecutionStepNone,
			ExecutionStepProgram,
			ExecutionStepCfgLocale
		};
		
		bool _isDefaultLanguage();
		void _setDefaultLanguage();
		void _setLanguagePanelLanguages(const wstring primaryCode, const wstring secondaryCode);
		bool _isLanguagePanelFirst();
		void _readPanelLanguageCode(wstring& language);
		bool _isAlreadyApplied();
		void _buildLanguagePanelPowerShellScript(const wstring primaryCode, const wstring secondaryCode, string& script);
		void _runLanguagePanelPowerShellScript(const string script);
		bool _isLangPackInstalledForLanguage(wstring langcode);
		bool _isDefaultLanguageForLanguage(wstring langcode);
		void _selectLanguagePackage();
		
		wstring m_filename;
		wstring m_scriptfile;
		IRunner* m_runner;
		IRegistry* m_registry;
		IWin32I18N* m_win32I18N;
		IOSVersion* m_OSVersion;
		ExecutionStep m_executionStep;
		wstring m_packageDownloadId;
		wstring m_packageLanguageCode;
};

