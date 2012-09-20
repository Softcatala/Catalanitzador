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

#include "Serializable.h"
#include "ActionStatus.h"
#include "ActionID.h"
#include "ActionGroup.h"
#include "StringConversion.h"
#include "DownloadManager.h"

#include <vector>
using namespace std;

//
// This abstract class defines the action interface for all actions
//
class Action : public Serializable
{
public:
		Action();
		Action(DownloadManager* downloadManager);
		virtual ~Action(){};

		// Get the name of action
		virtual wchar_t* GetName() = 0;

		// Get the detailed description to show to the user of action
		virtual wchar_t* GetDescription() = 0;

		// Unique ID that identifies the action
		virtual ActionID GetID() const = 0;

		// How we visually will group this action
		virtual ActionGroup GetGroup() const {return ActionGroupNone;}

		// If the action needs to download files to be completed (like language packages) or can run without
		// downloading files (changes in configuration). This is used to determine if Internet Connection is
		// needed to execute this action
		virtual bool IsDownloadNeed() {return true;}

		// If the action needs to be performed in this PC or not (already done, software not installed, etc)
		virtual bool IsNeed() = 0;

		// If the user needs to reboot the PC after executing the action to make the changes effective
		virtual bool IsRebootNeed() const { return false;}

		// Get the status of the action
		virtual ActionStatus GetStatus() { return status;}

		// Get the version of the application for which the action makes a change
		virtual const wchar_t* GetVersion() { return L"";}

		// Returns the reason why this action cannot be applied
		virtual wchar_t* GetCannotNotBeApplied() { return szCannotBeApplied;}

		// If a download is needed, executes the download of the files
		virtual bool Download(ProgressStatus, void *data) {return true;}

		// Executes the action and make the configuration changes effective
		virtual void Execute() = 0;

		// Serialize the changes into an XML file
		virtual void Serialize(ostream* stream);

		// This method is called to verify if the prerequirements to execute 
		// the action are satisfied, such as the software is installed, is
		// the right version, etc.
		virtual void CheckPrerequirements(Action * action){};

		// An action may depend in another action to be selected to be able to execute it
		// For example, Internet Explorer 6 language pack is part of the Windows language pack
		virtual ActionID DependsOn() const { return NoAction; };

		// If the action has license that the user needs to accept before installing
		// contains the Windows resouce ID of the license text
		virtual LPCWSTR GetLicenseID() { return NULL; };

		// Public utility methods
		void SetStatus(ActionStatus value);
		void GetLicense(wstring &license);
		bool HasLicense() { return GetLicenseID() != NULL; };

		Action* AnotherActionDependsOnMe(vector <Action *> * allActions);
		bool HasDependency(vector <Action *> * allActions);

protected:

		void _setStatusNotInstalled();
		wchar_t* _getStringFromResourceIDName(int nID, wchar_t* string);
		bool _doesDownloadExist();

		TCHAR szName[MAX_LOADSTRING];
		TCHAR szDescription[MAX_LOADSTRING];
		TCHAR szCannotBeApplied[MAX_LOADSTRING];
		ActionStatus status;
		DownloadManager* m_downloadManager;
};

