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
#include "InternetAccess.h"
#include "ProgressStatus.h"
#include "Serializable.h"
#include "ActionStatus.h"
#include "ActionID.h"


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
		virtual char* GetVersion() { return "";}
		virtual wchar_t* GetCannotNotBeApplied() { return szCannotBeApplied;}
		virtual void SetStatus(ActionStatus value) { status = value; }
		virtual bool Download(ProgressStatus, void *data) {return false;}
		virtual void Execute() = 0;
		virtual void Serialize(ostream* stream);
		virtual void CheckPrerequirements(Action * action){};
		virtual ActionID DependsOn() { return NoAction;};
		Action* AnotherActionDependsOnMe(vector <Action *> * allActions);
		bool HasDependency(vector <Action *> * allActions);

protected:
		wchar_t* _getStringFromResourceIDName(int nID, wchar_t* string);

		TCHAR szName[MAX_LOADSTRING];
		TCHAR szDescription[MAX_LOADSTRING];
		TCHAR szCannotBeApplied[MAX_LOADSTRING];
		ActionStatus status;
};

