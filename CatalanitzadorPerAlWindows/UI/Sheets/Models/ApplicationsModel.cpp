/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
#include "ApplicationsModel.h"
#include "ApplicationsPropertyPageUI.h"
#include "Runner.h"

// An index to ActionGroup
static const int groupNames [] = {IDS_GROUPNAME_NONE, IDS_GROUPNAME_WINDOWS, IDS_GROUPNAME_OFFICE, IDS_GROUPNAME_BROWSERS, IDS_GROUPNAME_INTERNET };


ApplicationsModel::ApplicationsModel(ApplicationExecutor* applicationExecutor) 
{
	m_actionsForUT = NULL;
	m_applicationExecutor = applicationExecutor; 
}


wstring ApplicationsModel::_getGroupName(ActionGroup actionGroup)
{
	wchar_t szGroupName[MAX_LOADSTRING];
	LoadString(GetModuleHandle(NULL), groupNames[actionGroup], szGroupName, MAX_LOADSTRING);

	return wstring(szGroupName);
}

vector <Action *> * ApplicationsModel::GetActions()
{
	if (m_actionsForUT != NULL)
		return m_actionsForUT;

	return m_applicationExecutor->GetActions();
}

void ApplicationsModel::_processDependantItems()
{
	for (unsigned int i = 0; i < m_items.size(); i++)
	{
		Action* itemAction = (Action *) m_items.at(i).GetData();

		if (m_items[i].GetIsGroupName() == false)
		{
			_processDependantItem(itemAction);
		}
	}
}

wstring ApplicationsModel::_getActionDisplayName(Action *action)
{
	wstring name = action->GetName();

	if (action->GetStatus() == Selected && action->HasLicense())
	{
		name+= L" *";
	}
	return name;
}


void ApplicationsModel::BuildListOfItems()
{
	for (int g = 0; g < ActionGroupLast; g++)
	{
		bool bFirstHit = false;

		for (unsigned int i = 0; i < GetActions()->size(); i++)
		{		
			Action* action = GetActions()->at(i);

			if (action->IsVisible() == false || action->GetGroup() != (ActionGroup)g)
				continue;

			if (bFirstHit == false)
			{				
				ApplicationItem applicationItem(_getGroupName((ActionGroup)g), NULL, true, true);
				m_items.push_back(applicationItem);
				bFirstHit = true;
			}

			bool disabled = action->GetStatus() == NotInstalled || 	action->GetStatus() == AlreadyApplied || 
				action->GetStatus() == CannotBeApplied;

			ApplicationItem applicationItem(_getActionDisplayName(action), action, false, disabled);
			applicationItem.SetImageIndex(_getImageIndex(action->GetStatus()));
			m_items.push_back(applicationItem);
		}
	}

	_processDependantItems();
}

int ApplicationsModel::_getItemIndexForItemData(void *data)
{
	for (unsigned int i = 0; i < m_items.size(); i++)
	{
		Action* itemAction = (Action *) m_items.at(i).GetData();
		if (itemAction == data)
		{
			return i;
		}
	}
	assert(false);
	return -1;
}

void ApplicationsModel::ProcessClickOnItem(ApplicationItem applicationItem)
{
	if (applicationItem.GetIsGroupName())
		return;

	Action* action = (Action*) applicationItem.GetData();

	switch (action->GetStatus()) {
	case NotSelected:
		action->SetStatus(Selected);
		break;
	case Selected:
		action->SetStatus(NotSelected);
		break;
	default:
		return; // Non selectable item
	}
	
	int itemIdx = _getItemIndexForItemData(action);

	if (itemIdx != -1)
	{
		applicationItem.SetImageIndex(_getImageIndex(action->GetStatus()));
		m_items[itemIdx] = applicationItem;
		m_applicationsView->UpdateItem(applicationItem);
	}

	_processDependantItem(action);
}

void ApplicationsModel::_processDependantItem(Action* action)
{	
	Action* dependant = action->AnotherActionDependsOnMe(GetActions());

	if (dependant == NULL)
		return;
	
	ActionStatus prevStatus = dependant->GetStatus();
	dependant->CheckPrerequirements(action);

	if (prevStatus == dependant->GetStatus())
		return;

	int itemIdx = _getItemIndexForItemData(dependant);

	if (itemIdx != -1)
	{
		ApplicationItem applicationItem;

		applicationItem = m_items.at(itemIdx);
		applicationItem.SetIsDisabled(dependant->IsNeed() == false);
		applicationItem.SetImageIndex(_getImageIndex(dependant->GetStatus()));
		m_items[itemIdx] = applicationItem;
		m_applicationsView->UpdateItem(applicationItem);
	}	
}

ImageIndex ApplicationsModel::_getImageIndex(ActionStatus status)
{
	switch (status)
	{
	case NotSelected:
		return ImageIndexNone;
	case Selected:
		return ImageIndexBlack;
	case CannotBeApplied:
	case NotInstalled:
		return ImageIndexGrey;
	case AlreadyApplied:
		return ImageIndexGreen;
	default:
		return ImageIndexNone;
	}
}

bool ApplicationsModel::_anyActionNeedsInternetConnection()
{
	bool needInet = false;

	for (unsigned int i = 0; i < m_items.size(); i++)
	{
		if (m_items[i].GetIsGroupName())
			continue;

		Action* action = (Action *) m_items.at(i).GetData();		

		if (action->GetStatus() == Selected && action->IsDownloadNeed())
		{
			needInet = true;
			break;
		}
	}	
	return needInet;
}

bool ApplicationsModel::ShouldShowNoInternetConnectionDialog()
{
	return _anyActionNeedsInternetConnection() && Inet::IsThereConnection() == false;
}

vector <ApplicationLegendItem> ApplicationsModel::GetLegendItems()
{
	vector <ApplicationLegendItem> applicationLegendItems;
	ActionStatus statuses [] = {Selected, AlreadyApplied, CannotBeApplied};
	int resources [] = {IDS_LEGEND_SELECTED, IDS_LEGEND_ALREADYAPPLIED, IDS_LEGEND_CANNOT};

	for (int l = 0; l <sizeof(statuses) / sizeof(statuses[0]); l++)
	{
		wchar_t szString [MAX_LOADSTRING];
		
		LoadString(GetModuleHandle(NULL), resources[l], szString, MAX_LOADSTRING);
		applicationLegendItems.push_back(ApplicationLegendItem(szString, _getImageIndex(statuses[l])));		
	}
	return applicationLegendItems;
}

bool ApplicationsModel::DoLicensesNeedToBeAccepted()
{	
	for (unsigned int i = 0; i < GetActions()->size (); i++)
	{
		Action* action = GetActions()->at(i);

		if (action->GetStatus() != Selected)
			continue;

		if (action->HasLicense())
			return true;	
	}
	return false;	
}

void ApplicationsModel::LogRunningProcesses()
{
	Runner runner;
	vector <wstring> names;
	wstring log;

	names = runner.GetRunningProcessesNames();

	for (unsigned int i = 0; i < names.size(); i++)
	{
		log += names[i];

		if (i +1 < names.size())
			log += L", ";
	}
	g_log.Log(L"ApplicationsModel::LogRunningProcesses: %s", (wchar_t *) log.c_str());
}


vector <wstring> ApplicationsModel::GetManualSteps()
{	
	vector <wstring> steps;

	for (unsigned int i = 0; i < GetActions()->size (); i++)
	{
		Action* action = GetActions()->at(i);
		if (action->GetStatus() != Selected)
			continue;

		const wchar_t* manualSteps = action->GetManualStep();

		if (wcslen(manualSteps) == 0)
			continue;
		
		wstring step;
		wchar_t szMessage[MAX_LOADSTRING];
		wchar_t szFormatted[MAX_LOADSTRING];
		
		LoadString(GetModuleHandle(NULL), IDS_MANUAL_STEPS_WARNING, szMessage, MAX_LOADSTRING);
		wsprintf(szFormatted, szMessage, action->GetName());
		step = szFormatted;

		step += manualSteps;
		steps.push_back(step);
	}

	return steps;
}
