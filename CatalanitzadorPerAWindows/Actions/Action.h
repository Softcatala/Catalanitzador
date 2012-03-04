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

#include <windows.h>
#include "DownloadInet.h"
#include "Serializable.h"
#include "ActionStatus.h"
#include "ActionID.h"
#include "ActionDownload.h"
#include "StringConversion.h"

#include <vector>
using namespace std;

class _APICALL Action : public Serializable
{
public:
		Action();
		virtual ~Action(){};
		virtual wchar_t* GetName() = 0;
		virtual wchar_t* GetDescription() = 0;
		virtual int GetID() = 0;
		virtual bool IsDownloadNeed() {return true;}
		virtual bool IsNeed() = 0;
		virtual bool IsRebootNeed() { return false;};
		virtual ActionStatus GetStatus() { return status;}
		virtual const char* GetVersion() { return "";}
		virtual wchar_t* GetCannotNotBeApplied() { return szCannotBeApplied;}		
		virtual bool Download(ProgressStatus, void *data) {return true;}
		virtual void Execute() = 0;
		virtual void Serialize(ostream* stream);
		virtual void CheckPrerequirements(Action * action){};
		virtual ActionID DependsOn() { return NoAction; };
		virtual LPCWSTR GetLicenseID() { return NULL; };
		virtual DWORD GetProcessIDForRunningApp() {return NULL;}

		void SetStatus(ActionStatus value);
		void GetLicense(wstring &license);
		bool HasLicense() { return GetLicenseID() != NULL; };

		Action* AnotherActionDependsOnMe(vector <Action *> * allActions);
		bool HasDependency(vector <Action *> * allActions);

protected:

		bool _getFile(DownloadID downloadID, wstring file, ProgressStatus progress, void *data);
		wchar_t* _getStringFromResourceIDName(int nID, wchar_t* string);

		TCHAR szName[MAX_LOADSTRING];
		TCHAR szDescription[MAX_LOADSTRING];
		TCHAR szCannotBeApplied[MAX_LOADSTRING];
		ActionStatus status;
		ActionDownload m_actionDownload;
};

