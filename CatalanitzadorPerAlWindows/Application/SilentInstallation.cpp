/* 
 * Copyright (C) 2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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

#include "stdafx.h"

#include "SilentInstallation.h"
#include "ActionExecution.h"
#include "ApplicationExecutor.h"

static void _notifyExecutionStarts(NotifyActionData* notifyActionData) {}
static void _notifyDownloadCompleted(NotifyActionData* notifyActionData) { }
static void _notifyExecutionCompleted(NotifyActionData* notifyActionData) { }

static bool _notifyDownloadError(NotifyActionData* notifyActionData)
{
	return true;
}

static bool _downloadStatus(int total, int current, void *data) 
{	
	return true;
}


void SilentInstallation::_unSelectRunningActions(vector <Action *>* actions)
{
	for (unsigned int i = 0; i < actions->size(); i++)
	{
		Action* action = actions->at(i);

		if (action->GetStatus() != Selected)
			continue;
		
		ActionExecution* execution = dynamic_cast<ActionExecution*>(action);

		if (execution == NULL)
			continue;

		ExecutionProcess process = execution->GetExecutingProcess();

		if (process.IsEmpty())
			continue;
		
		action->SetStatus(NotSelected);

		g_log.Log(L"SilentInstallation::_unSelectRunningActions. Action '%s' has running applications. Unselecting it.",
			action->GetName());
	}
}


void SilentInstallation::Run()
{
	ApplicationExecutor applicationExecutor;
	Serializer serializer;
	
	applicationExecutor.CheckPrerequirements();
	applicationExecutor.SelectDefaultActions();
	_unSelectRunningActions(applicationExecutor.GetActions());

	applicationExecutor.SetSerializer(&serializer);
	applicationExecutor.PrepareStart();
	applicationExecutor.Start(_notifyExecutionStarts, _downloadStatus, _notifyDownloadError, _notifyDownloadCompleted, _notifyExecutionCompleted, this);
	applicationExecutor.SerializeOptionsSendStatistics();
	applicationExecutor.WaitForStatisticsToCompleteOrTimeOut();
}

