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
#include "RemoteURLs.h"
#include "FinishModel.h"
#include "ActionTest.h"
#include "Url.h"

using ::testing::StrCaseEq;


class FinishModelTest : public FinishModel
{
public:

	virtual void _shellExecuteURL(wstring url)
	{
		m_url = url;
	}

	wstring GetURL() {return m_url;}

private:

	wstring m_url;
};

class ActionTestSetStatus : public ActionTest
{
public:
		virtual void SetStatus(ActionStatus value)
		{
			status = value;
		}
};

class ActionTestRebootNeed : public ActionTest
{
public:
		virtual bool IsRebootNeed() const { return true;}
};

TEST(FinishModelTest, _calculateIndicatorsForProgressBar_50)
{
	FinishModelTest finishModel;
	vector <Action *> actions;	
	finishModel.SetActions(&actions);
		
	ActionTestSetStatus actionTestSuccessful;
	ActionTestSetStatus actionTestFinishedWithError;

	actionTestSuccessful.SetStatus(Successful);
	actionTestFinishedWithError.SetStatus(FinishedWithError);
	actions.push_back((Action *)&actionTestSuccessful);
	actions.push_back((Action *)&actionTestFinishedWithError);
	
	EXPECT_EQ(50.0, finishModel.GetCompletionPercentage());
	EXPECT_TRUE(finishModel.HasErrors());
}

TEST(FinishModelTest, _calculateIndicatorsForProgressBar_0)
{
	FinishModelTest finishModel;
	vector <Action *> actions;	
	finishModel.SetActions(&actions);
	
	ActionTestSetStatus actionTestFinishedWithError;	
	actionTestFinishedWithError.SetStatus(FinishedWithError);	
	actions.push_back((Action *)&actionTestFinishedWithError);
	
	EXPECT_EQ(0.0, finishModel.GetCompletionPercentage());
	EXPECT_TRUE(finishModel.HasErrors());
}

TEST(FinishModelTest, _calculateIndicatorsForProgressBar_100)
{
	FinishModelTest finishModel;
	vector <Action *> actions;	
	finishModel.SetActions(&actions);

	ActionTestSetStatus actionTestSuccessful;
	actionTestSuccessful.SetStatus(Successful);	
	actions.push_back((Action *)&actionTestSuccessful);
	
	EXPECT_EQ(100.0, finishModel.GetCompletionPercentage());
	EXPECT_FALSE(finishModel.HasErrors());
}

TEST(FinishModelTest, OpenTwitter)
{
	#define EXPECTED_TWITTER_URL_HEAD L"http://twitter.com/share?text=Jo%20ja%20he%20catalanitzat%20el%20meu%20ordinador%21%20Tu%20tamb%C3%A9%20pots%20amb%20el%20Catalanitzador%20de%20Softcatal%C3%A0%3A%20"
	#define EXPECTED_TWITTER_URL_TAIL L"%20%23catalanitzador";
	wstring expectedUrl, encodedUrl;
	
	Url::EncodeParameter(wstring(APPLICATON_WEBSITE), encodedUrl);
	expectedUrl = EXPECTED_TWITTER_URL_HEAD;
	expectedUrl += encodedUrl;
	expectedUrl += EXPECTED_TWITTER_URL_TAIL;
		
	FinishModelTest finishModel;
	finishModel.OpenTwitter();
	wstring url = finishModel.GetURL();
	
	EXPECT_THAT(url, StrCaseEq(expectedUrl.c_str()));
}

TEST(FinishModelTest, OpenFacebook)
{
	#define EXPECTED_FACEBOOK_URL_HEAD L"http://ca-es.facebook.com/sharer.php?u="

	wstring expectedUrl;
	
	expectedUrl = EXPECTED_FACEBOOK_URL_HEAD;
	expectedUrl += APPLICATON_WEBSITE;
		
	FinishModelTest finishModel;
	finishModel.OpenFacebook();
	wstring url = finishModel.GetURL();
	
	EXPECT_THAT(url, StrCaseEq(expectedUrl.c_str()));
}

TEST(FinishModelTest, OpenGooglePlus)
{
	#define EXPECTED_PLUS_URL_HEAD L"http://plusone.google.com/_/+1/confirm?hl=ca&url="

	wstring expectedUrl;
	
	expectedUrl = EXPECTED_PLUS_URL_HEAD;
	expectedUrl += APPLICATON_WEBSITE;	
		
	FinishModelTest finishModel;
	finishModel.OpenGooglePlus();
	wstring url = finishModel.GetURL();
	
	EXPECT_THAT(url, StrCaseEq(expectedUrl.c_str()));
}

TEST(FinishModelTest, OpenMailTo)
{
	FinishModelTest finishModel;
	finishModel.OpenMailTo();
	wstring url = finishModel.GetURL();
	
	EXPECT_THAT(url, StrCaseEq(CONTACT_EMAIL));
}

TEST(FinishModelTest, IsRebootNeed_No)
{
	FinishModelTest finishModel;
	vector <Action *> actions;	
	finishModel.SetActions(&actions);

	ActionTest actionTest;
	actions.push_back((Action *)&actionTest);
	
	EXPECT_FALSE(finishModel.IsRebootNeed());
}

TEST(FinishModelTest, IsRebootNeed_Yes)
{
	FinishModelTest finishModel;
	vector <Action *> actions;	
	finishModel.SetActions(&actions);

	ActionTest actionTest;
	ActionTestRebootNeed actionTestRebootNeed;
	actions.push_back((Action *)&actionTest);
	actions.push_back((Action *)&actionTestRebootNeed);
	
	EXPECT_TRUE(finishModel.IsRebootNeed());
}