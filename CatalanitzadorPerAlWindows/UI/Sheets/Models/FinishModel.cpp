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
#include "Version.h"
#include <math.h>
#include <Shellapi.h>

#include "Authorization.h"
#include "FinishModel.h"
#include "Url.h"
#include "Option.h"
#include "OptionID.h"
#include "Options.h"


FinishModel::FinishModel()
{
	m_errors = false;
	m_uploadStatistics = NULL;
	m_completionPercentage = -1;
	m_sentStats = false;
	m_openTwitter = false;
	m_openFacebook = false;
	m_openGooglePlus = false;
}

FinishModel::~FinishModel()
{
	if (m_uploadStatistics != NULL)
	{
		delete m_uploadStatistics;
		m_uploadStatistics = NULL;
	}

#if !DEVELOPMENT_VERSION
	if (m_xmlFile.size() > 0)
	{
		DeleteFile(m_xmlFile.c_str());
	}
#endif
}

void FinishModel::_calculateIndicatorsForProgressBar()
{
	Action* action;
	int done, doable;
	
	doable = done = 0;
	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

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
				m_errors = true;
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

	return m_errors; 
}

bool FinishModel::IsRebootNeed()
{
	Action* action;

	for (unsigned int i = 0; i < m_actions->size(); i++)
	{
		action = m_actions->at(i);

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

void FinishModel::_saveToDisk()
{
	wchar_t szXML[MAX_PATH];

	GetTempPath(MAX_PATH, szXML);	
	wcscat_s(szXML, L"statistics.xml");
	m_xmlFile = szXML;
	m_serializer->SaveToFile(m_xmlFile);
}
void FinishModel::SerializeOptionsSendStatistics()
{
	if (m_sentStats == true)
		return;

	_serializeOptions();
	_sendStatistics();
	m_sentStats = true;
}

void FinishModel::_sendStatistics()
{
	if (*m_pbSendStats)
	{
		m_uploadStatistics = new UploadStatisticsThread(&m_httpFormInet, m_serializer, m_errors);
		m_uploadStatistics->Start();
	}

	_saveToDisk();
}

void FinishModel::WaitForStatisticsToCompleteOrTimeOut()
{
	if (m_uploadStatistics!= NULL)
		m_uploadStatistics->Wait();
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
	wcscpy_s(szURL, L"http://twitter.com/share?text=");
	wcscat_s(szURL, parameter.c_str());
	_shellExecuteURL(szURL);
	m_openTwitter = true;
}

void FinishModel::OpenFacebook()
{
	wchar_t szURL[1024];	

	// See: http://developers.facebook.com/docs/share/
	swprintf_s(szURL, L"http://ca-es.facebook.com/sharer.php?u=%s", APPLICATON_WEBSITE);
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

void FinishModel::_serializeOptions()
{
	Options options;
	Option sysOption(OptionSystemRestore, *m_pSystemRestore);
	Option dialectOption(OptionDialect, *m_pbDialectalVariant);
	Option showSecDlgOption(OptionShowSecDlg, *m_pbShowSecDlg);
	Option twitterOption(OptionShareTwitter, m_openTwitter);
	Option faceBookOption(OptionShareFacebook, m_openFacebook);
	Option googlePlusSecDlgOption(OptionShareGooglePlus, m_openGooglePlus);

	options.Add(sysOption);
	options.Add(dialectOption);
	options.Add(showSecDlgOption);
	options.Add(twitterOption);
	options.Add(faceBookOption);
	options.Add(googlePlusSecDlgOption);
	options.Serialize(m_serializer->GetStream());
	m_serializer->CloseHeader();
}
