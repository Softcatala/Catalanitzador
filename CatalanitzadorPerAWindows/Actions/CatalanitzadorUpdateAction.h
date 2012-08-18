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

class _APICALL CatalanitzadorUpdateAction : public Action
{
public:
		CatalanitzadorUpdateAction(IRunner* runner, DownloadManager* downloadManager);

		virtual wchar_t* GetName();
		virtual wchar_t* GetDescription();
		virtual ActionID GetID() const { return CatalanitzadorUpdate;};
		virtual ActionGroup GetGroup() const {return ActionGroupNone;}
		virtual bool Download(ProgressStatus progress, void *data);
		virtual bool IsNeed();
		virtual void Execute();
		virtual const wchar_t* GetVersion();
		virtual void CatalanitzadorUpdateAction::CheckPrerequirements(Action * action);
		void SetVersion(wstring version) {m_version = version;}

private:

		bool _isRunningInstanceUpToDate();
	
		IRunner* m_runner;
		wstring m_filename;
		wstring m_version;
};

