/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

class _APICALL WindowsLPIAction : public Action
{
public:
		WindowsLPIAction(IOSVersion* OSVersion, IRegistry* registry, IWin32I18N* win32I18N, IRunner* runner);
		~WindowsLPIAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return WindowsLPI;};
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
		bool _isDefaultLanguage();
		DownloadID _getDownloadID();
		virtual bool _isWindowsValidated();

private:
		
		bool _isWindowsXPValidated();
		void _setDefaultLanguage();
		bool _isASupportedSystemLanguage();

		wchar_t m_szFilename[MAX_PATH];
		IRunner* m_runner;
		IRegistry* m_registry;
		IWin32I18N* m_win32I18N;
		IOSVersion* m_OSVersion;
};

