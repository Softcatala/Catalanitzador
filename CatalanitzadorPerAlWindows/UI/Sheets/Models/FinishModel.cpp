/* 
 * Copyright (C) 2012-2014 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "Version.h"
#include <math.h>
#include <Shellapi.h>

#include "Authorization.h"
#include "FinishModel.h"
#include "Url.h"


FinishModel::FinishModel()
{
	m_applicationExecutor = NULL;
	_commonConstructor();
}

FinishModel::FinishModel(ApplicationExecutor* applicationExecutor)
{
	_commonConstructor();
	m_applicationExecutor = applicationExecutor;
}


void FinishModel::_commonConstructor()
{
	m_completionPercentage = -1;
	m_openTwitter = false;
	m_openFacebook = false;
	m_openGooglePlus = false;
	m_actionsForUT = NULL;
}

vector <Action *> * FinishModel::GetActions()
{
	if (m_actionsForUT != NULL)
		return m_actionsForUT;

	return m_applicationExecutor->GetActions();
}

void FinishModel::_calculateIndicatorsForProgressBar()
{
	Action* action;
	int done, doable;
	
	doable = done = 0;
	for (unsigned int i = 0; i < GetActions()->size(); i++)
	{
		action = GetActions()->at(i);

		// Not visible action for which we do not want to show statistics
		if (action->IsVisible() == false)
			continue;

		switch (action->GetStatus())
		{			
			case AlreadyApplied:
			case Successful:
				doable++;
				done++;
				break;
			case NotSelected:
				doable++;
				break;
			case FinishedWithError:
				doable++;
				break;
			default:
				break;
		}
	}

	m_completionPercentage = doable != 0 ? floor ((float) done/ (float)doable * 100.0f) : 0;
}

double FinishModel::GetCompletionPercentage() 
{
	if (m_completionPercentage == -1)
	{
		_calculateIndicatorsForProgressBar();
	}

	return m_completionPercentage;
}

bool FinishModel::HasErrors() 
{
	if (m_completionPercentage == -1)
	{
		_calculateIndicatorsForProgressBar();
	}

	return m_applicationExecutor->HasErrors();
}

bool FinishModel::IsRebootNeed()
{
	Action* action;

	for (unsigned int i = 0; i < GetActions()->size(); i++)
	{
		action = GetActions()->at(i);

		if (action->IsRebootNeed())
		{
			return true;
		}
	}
	return false;
}

void FinishModel::Reboot()
{
	Authorization::RequestShutdownPrivileges();
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

void FinishModel::_shellExecuteURL(wstring url)
{
	ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void FinishModel::OpenTwitter()
{
	wstring parameter;
	wchar_t szURL[1024];
	wchar_t szText[MAX_LOADSTRING], szString[MAX_LOADSTRING];

	LoadString(GetModuleHandle(NULL), IDS_SOCIALNETWORKS_TWITTER, szString, MAX_LOADSTRING);
	swprintf_s(szText, szString, APPLICATON_WEBSITE);
	
	Url::EncodeParameter(wstring(szText), parameter);
	wcscpy_s(szURL, L"https://twitter.com/share?text=");
	wcscat_s(szURL, parameter.c_str());
	_shellExecuteURL(szURL);
	m_openTwitter = true;
}

void FinishModel::OpenFacebook()
{
	wchar_t szURL[1024];	

	// See: https://developers.facebook.com/docs/share/
	swprintf_s(szURL, L"https://ca-es.facebook.com/sharer.php?u=%s", APPLICATON_WEBSITE);
	_shellExecuteURL(szURL);
	m_openFacebook = true;
}

void FinishModel::OpenGooglePlus()
{
	wchar_t szURL[1024];
	
	// See: https://developers.google.com/+/plugins/+1button/		
	swprintf_s(szURL, L"https://plus.google.com/share?url=%s", APPLICATON_WEBSITE);
	_shellExecuteURL(szURL);
	m_openGooglePlus = true;
}

void FinishModel::OpenMailTo()
{
	_shellExecuteURL(CONTACT_EMAIL);
}

void FinishModel::SerializeOptionsSendStatistics() 
{
	_setSocialOptions();
	m_applicationExecutor->SerializeOptionsSendStatistics(); 
}

void FinishModel::_setSocialOptions()
{
	m_applicationExecutor->SetOption(Option(OptionShareTwitter, m_openTwitter));
	m_applicationExecutor->SetOption(Option(OptionShareFacebook, m_openFacebook));
	m_applicationExecutor->SetOption(Option(OptionShareGooglePlus, m_openGooglePlus));
}


