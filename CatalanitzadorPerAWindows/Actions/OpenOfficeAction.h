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
#include "IOSVersion.h"
#include "IRegistry.h"
#include "OSVersion.h"
#include "Runner.h"
#include "XmlParser.h"

#define OPENOFFICCE_PROGRAM_REGKEY L"SOFTWARE\\OpenOffice.org\\OpenOffice.org"

class _APICALL OpenOfficeAction : public Action
{
public:
		OpenOfficeAction(IRegistry* registry, IRunner* runner);
		~OpenOfficeAction();

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return OpenOffice;};
		virtual ActionGroup GetGroup() const {return ActionGroupOfficeAutomation;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual ActionStatus GetStatus();
		virtual const wchar_t* GetVersion() {return m_version.c_str();}
		virtual void CheckPrerequirements(Action * action);
		virtual bool IsExecuting();
		virtual void FinishExecution();

protected:

		void _readVersionInstalled();
		void _setDefaultLanguage();
		bool _isDefaultLanguage();
		virtual void _getPreferencesFile(wstring& location);

private:				
		bool _extractCabFile(wchar_t * file, wchar_t * path);
		void _removeCabTempFiles();
		bool _isLangPackInstalled();
		DownloadID _getDownloadID();
		static bool _readNodeCallback(XmlNode node, void *data);


		wstring m_version;
		IRunner* m_runner;
		IRegistry* m_registry;
		wchar_t m_szFilename[MAX_PATH];
		wchar_t m_szTempPath[MAX_PATH];
		wchar_t m_szTempPathCAB[MAX_PATH];
};

