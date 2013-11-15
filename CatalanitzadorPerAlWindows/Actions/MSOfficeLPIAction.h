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
#include "IRunner.h"
#include "IOSVersion.h"
#include "IRegistry.h"
#include "TriBool.h"

enum MSOfficeVersion
{
	MSOfficeUnKnown,
	NoMSOffice,
	MSOffice2003,
	MSOffice2007,
	MSOffice2010,
	MSOffice2010_64,
	MSOffice2013,
	MSOffice2013_64
};


struct RegKeyVersion
{
	wchar_t* VersionNumber;
	wchar_t* InstalledLangMapKey;
	bool InstalledLangMapKeyIsDWord;
};

class MSOfficeLPIAction : public Action
{
public:
		MSOfficeLPIAction(IRegistry* registry, IRunner* runner, DownloadManager* downloadManager);
		~MSOfficeLPIAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return MSOfficeLPIActionID;};
		virtual ActionGroup GetGroup() const {return ActionGroupOfficeAutomation;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion();
		virtual LPCWSTR GetLicenseID();
		virtual void CheckPrerequirements(Action * action);

protected:
		
		MSOfficeVersion _getVersionInstalled();
		bool _isLangPackInstalled();
		wchar_t* _getDownloadID();

private:

		enum ExecutionStep
		{
			ExecutionStepNone,
			ExecutionStep1,
			ExecutionStep2	
		};
		
		bool _isVersionInstalled(RegKeyVersion regkeys, bool b64bits);
		void _readVersionInstalled();
		bool _isLangPackForVersionInstalled(RegKeyVersion regkeys, bool b64bits);
		bool _extractCabFile(wchar_t * file, wchar_t * path);
		void _setDefaultLanguage();		
		void _removeOffice2003TempFiles();
		RegKeyVersion _getRegKeys();
		bool _needsInstallConnector();
		bool _executeInstallConnector();
		void _readIsLangPackInstalled();

		TriBool m_bLangPackInstalled;
		bool m_bLangPackInstalled64bits;
		wchar_t m_szFullFilename[MAX_PATH];
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szTempPath[MAX_PATH];
		wchar_t m_szTempPath2003[MAX_PATH];
		MSOfficeVersion m_MSVersion;
		wstring m_connectorFile;
		ExecutionStep m_executionStep;
		IRunner* m_runner;
		IRegistry* m_registry;
		wstring m_msiexecLog;
};

