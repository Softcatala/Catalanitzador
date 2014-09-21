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
#include "ApplicationItem.h"
#include "ApplicationLegendItem.h"
#include "IApplicationsPropertyPageUI.h"
#include "ApplicationExecutor.h"

#include <string>
#include <vector>

using namespace std;

class ApplicationsModel
{
public:

		ApplicationsModel(ApplicationExecutor* applicationExecutor) 
		{
			m_actionsForUT = NULL;
			m_applicationExecutor = applicationExecutor; 
		}

		void SetActionsForUT(vector <Action *> * value) {m_actionsForUT =  value; }
		vector <Action *> * GetActions();
		void SetDialectVariant(bool dialectVariant) {m_applicationExecutor->SetDialectVariant(dialectVariant); }
		
		void SelectDefaultActions() {m_applicationExecutor->SelectDefaultActions(); }
		void BuildListOfItems();
		vector <ApplicationItem> & GetItems() {return m_items;}
		void ProcessClickOnItem(ApplicationItem applicationItem);
		bool ShouldShowNoInternetConnectionDialog();
		void SetView(IApplicationsPropertyPageUI* applicationsView) {m_applicationsView = applicationsView;}
		vector <ApplicationLegendItem> GetLegendItems();
		bool DoLicensesNeedToBeAccepted();
		void LogRunningProcesses();
		vector <wstring> GetManualSteps();

protected:

		bool _anyActionNeedsInternetConnection();

private:

		wstring _getActionDisplayName(Action *action);
		void _processDependantItem(Action* action);
		ImageIndex _getImageIndex(ActionStatus status);
		wstring _getGroupName(ActionGroup actionGroup);
		int _getItemIndexForItemData(void *data);
		void _processDependantItems();
		
		vector <Action *> * m_actionsForUT;
		vector <ApplicationItem> m_items;
		IApplicationsPropertyPageUI* m_applicationsView;
		ApplicationExecutor* m_applicationExecutor;
};
