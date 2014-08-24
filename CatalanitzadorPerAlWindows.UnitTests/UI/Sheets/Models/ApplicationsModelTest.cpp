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

#include "stdafx.h"
#include "Defines.h"
#include "ApplicationsModel.h"
#include "ActionTest.h"
#include "ConfigurationInstance.h"

class ActionGroupWindowsTest : public ActionTest
{
public:

		ActionGroupWindowsTest()
		{
			m_downloadManager =  &m_testDownloadManager;
		}

		virtual ActionGroup GetGroup() const {return ActionGroupWindows;}
		virtual ActionID GetID() const {return WindowsLPIActionID;}

private:
		DownloadManager m_testDownloadManager;
		
};

class ApplicationsModelTest : public ApplicationsModel
{
public:

	ApplicationsModelTest() 
	{
		ConfigurationRemote remote;
		ConfigurationInstance::Get().SetRemote(remote);
	}

	~ApplicationsModelTest() 
	{
		ConfigurationInstance::Reset();
	}

	using ApplicationsModel::_anyActionNeedsInternetConnection;
};

class ActionGroupInternetTest : public ActionTest
{
public:
		virtual ActionGroup GetGroup() const {return ActionGroupInternet;}
};

class ActionNotNeededTest : public ActionGroupWindowsTest
{
public:
		virtual bool IsNeed() {return false;}
};

class ActionNoDownloadTest : public ActionGroupWindowsTest
{
public:
		virtual bool IsDownloadNeed() {return false;}
};

class ActionHasLicense : public ActionTest
{
public:
		virtual LPCWSTR GetLicenseID() { return MAKEINTRESOURCE(0x1); };
};

#define TEXT_STEP L"Text step in action"

class ActionHasStep : public ActionTest
{
public:

		virtual const wchar_t* GetManualStep() { return TEXT_STEP;}
};

#define CreateApplicationModelObject \
	ApplicationsModelTest applicationModel; \
	vector <Action *> actions; \
	vector <ApplicationItem> applicationItems; \
	applicationModel.SetActions(&actions);


TEST(ApplicationsModelTest, BuildListOfItems_SingleGroup)
{
	CreateApplicationModelObject;	
	ActionGroupWindowsTest actionGroupWindows1;
	ActionGroupWindowsTest actionGroupWindows2;	

	actions.push_back((Action *)&actionGroupWindows1);
	actions.push_back((Action *)&actionGroupWindows2);	
	applicationModel.BuildListOfItems();
	applicationItems = applicationModel.GetItems();

	EXPECT_EQ(3, applicationItems.size());
	EXPECT_TRUE(applicationItems.at(0).GetIsGroupName());
	EXPECT_FALSE(applicationItems.at(1).GetIsGroupName());
	EXPECT_FALSE(applicationItems.at(2).GetIsGroupName());
}

TEST(ApplicationsModelTest, BuildListOfItems_MultipleGroup)
{
	CreateApplicationModelObject;
	ActionGroupWindowsTest actionGroupWindows1;
	ActionGroupWindowsTest actionGroupWindows2;	

	actions.push_back((Action *)&actionGroupWindows1);
	actions.push_back((Action *)&actionGroupWindows2);
	applicationModel.BuildListOfItems();
	applicationItems = applicationModel.GetItems();

	EXPECT_EQ(3, applicationItems.size());
	EXPECT_TRUE(applicationItems.at(0).GetIsGroupName());
	EXPECT_FALSE(applicationItems.at(1).GetIsGroupName());
	EXPECT_FALSE(applicationItems.at(2).GetIsGroupName());
}

TEST(ApplicationsModelTest, BuildListOfItems_Selected)
{
	CreateApplicationModelObject;
	ActionGroupWindowsTest actionGroupWindows;
	ActionNotNeededTest actionNotNeeded;
	
	actions.push_back((Action *)&actionGroupWindows);
	actions.push_back((Action *)&actionNotNeeded);
	applicationModel.BuildListOfItems();
	applicationItems = applicationModel.GetItems();

	EXPECT_EQ(3, applicationItems.size());
	EXPECT_FALSE(applicationItems.at(1).GetIsGroupName());
	EXPECT_FALSE(applicationItems.at(1).GetIsDisabled());
	EXPECT_FALSE(applicationItems.at(2).GetIsGroupName());
	EXPECT_TRUE(applicationItems.at(2).GetIsDisabled());
}

TEST(ApplicationsModelTest, BuildListOfItems_Configuration)
{
	CreateApplicationModelObject;
	ActionNotNeededTest actionNotNeeded;
	ActionNotNeededTest actionNotNeededConfiguration;

	// Configure actionNotNeededConfiguration to be NotSelected
	ConfigurationRemote remote;
	ConfigurationFileActionDownloads configurationFileActionDownloads;
	remote.AddFileActionDownloads(configurationFileActionDownloads);
	configurationFileActionDownloads.SetActionID(WindowsLPIActionID);
	configurationFileActionDownloads.SetActionDefaultStatus(NotSelected);
	ConfigurationInstance::Get().SetRemote(remote);
	
	actions.push_back((Action *)&actionNotNeeded);
	actions.push_back((Action *)&actionNotNeededConfiguration);	
	applicationModel.BuildListOfItems();
	applicationItems = applicationModel.GetItems();
	
	EXPECT_THAT(actionNotNeededConfiguration.GetStatus(), NotSelected);	
}

TEST(ApplicationsModelTest, DoLicensesNeedToBeAccepted_NotAccepted)
{
	CreateApplicationModelObject;
	ActionGroupWindowsTest actionGroupWindows1;

	actionGroupWindows1.SetStatus(Selected);
	actions.push_back((Action *)&actionGroupWindows1);
	
	EXPECT_FALSE(applicationModel.DoLicensesNeedToBeAccepted());	
}

TEST(ApplicationsModelTest, DoLicensesNeedToBeAccepted_Accepted)
{
	CreateApplicationModelObject;
	ActionHasLicense actionHasLicense;

	actionHasLicense.SetStatus(Selected);
	actions.push_back((Action *)&actionHasLicense);	
	
	EXPECT_TRUE(applicationModel.DoLicensesNeedToBeAccepted());
}

TEST(ApplicationsModelTest, _anyActionNeedsInternetConnection_True)
{
	CreateApplicationModelObject;
	ActionGroupWindowsTest actionGroupWindowsTest;

	actionGroupWindowsTest.SetStatus(Selected);
	actions.push_back((Action *)&actionGroupWindowsTest);
	applicationModel.BuildListOfItems();
	
	EXPECT_TRUE(applicationModel._anyActionNeedsInternetConnection());
}

TEST(ApplicationsModelTest, _anyActionNeedsInternetConnection_False)
{
	CreateApplicationModelObject;
	ActionNoDownloadTest actionNoDownloadTest;

	actionNoDownloadTest.SetStatus(Selected);
	actions.push_back((Action *)&actionNoDownloadTest);
	applicationModel.BuildListOfItems();
	
	EXPECT_FALSE(applicationModel._anyActionNeedsInternetConnection());	
}


TEST(ApplicationsModelTest, DoLicensesNeedToBeAccepted_GetManualSteps_No)
{
	CreateApplicationModelObject;
	ActionHasLicense actionHasLicense;

	actionHasLicense.SetStatus(Selected);
	actions.push_back((Action *)&actionHasLicense);	

	vector <wstring> steps = applicationModel.GetManualSteps();
	
	EXPECT_THAT(0, steps.size());
}

TEST(ApplicationsModelTest, DoLicensesNeedToBeAccepted_GetManualSteps_Yes)
{
	CreateApplicationModelObject;
	ActionHasStep actionHasStep;

	actionHasStep.SetStatus(Selected);
	actions.push_back((Action *)&actionHasStep);

	vector <wstring> steps = applicationModel.GetManualSteps();
	
	EXPECT_THAT(1, steps.size());
	EXPECT_TRUE(steps[0].find(TEXT_STEP) != string::npos);
}
