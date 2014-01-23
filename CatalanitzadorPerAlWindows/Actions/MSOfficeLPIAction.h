/* 
 * Copyright (C) 2011-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "MultipleDownloads.h"
#include "MSOffice.h"
#include "IOSVersion.h"

class MSOfficeLPIAction : public Action
{
public:
		MSOfficeLPIAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner, DownloadManager* downloadManager);
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
		virtual void Serialize(ostream* stream);

private:

		OutLookHotmailConnector* _getOutLookHotmailConnector();

		enum ExecutionStep
		{
			ExecutionStepNone,
			ExecutionStepMSOffice,
			ExecutionStepOutLookConnector,
			ExecutionCompleted
		};
		
		ExecutionStep m_executionStep;
		unsigned int m_executionStepIndex;
		IRunner* m_runner;
		IRegistry* m_registry;
		IOSVersion* m_OSVersion;
		MultipleDownloads m_multipleDownloads;
		OutLookHotmailConnector* m_OutLookHotmailConnector;
		vector <MSOffice> m_MSOffices;
};

