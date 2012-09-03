/* 
 * Copyright (C) 2011 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Runner.h"
#include "IOSVersion.h"
#include "InternetExplorerVersion.h"
#include "IFileVersionInfo.h"

class IELPIAction : public Action
{
public:	

		IELPIAction(IOSVersion* OSVersion, IRunner* runner, IFileVersionInfo* fileVersionInfo);
		~IELPIAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return IELPI;};
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();		
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual void CheckPrerequirements(Action * action);
		virtual ActionID DependsOn() const { return WindowsLPI;};
		virtual LPCWSTR GetLicenseID();

		enum Prerequirements
		{
			PrerequirementsOk,
			AppliedInWinLPI,
			NeedsWinLPI,
			NoLangPackAvailable,
			UnknownIEVersion,
		};

protected:
		
		virtual InternetExplorerVersion::IEVersion _getIEVersion();		
		Prerequirements _checkPrerequirementsDependand(Action * action);
		Prerequirements _checkPrerequirements();

private:

		wchar_t* _getDownloadID();
		wchar_t* _getDownloadIDIE8();
		wchar_t* _getDownloadIDIE9();
		bool _isLangPackInstalled();
		bool _is64BitsPackage();
		bool _createTempDirectory();
		bool _wasInstalled();

		wchar_t m_filename[MAX_PATH];
		wchar_t m_szTempDir[MAX_PATH];		
		IRunner* m_runner;
		IOSVersion* m_OSVersion;
		InternetExplorerVersion m_explorerVersion;
};

