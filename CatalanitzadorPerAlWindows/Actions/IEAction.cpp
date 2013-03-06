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
#include "IEAction.h"

IEAction::IEAction(IRegistry* registry, IRunner* runner, IFileVersionInfo* fileVersionInfo, IOSVersion* OSVersion, DownloadManager* downloadManager) : Action(downloadManager)
{
	m_registry = registry;
	m_runner = runner;
	m_OSVersion = OSVersion;
	m_fileVersionInfo = fileVersionInfo;
	m_LPIAction = NULL;
	m_acceptLanguagesAction = NULL;
	m_doAcceptLanguagesAction = false;
	m_doLPIAction = false;
}

IEAction::~IEAction()
{
	if (m_LPIAction)
		delete m_LPIAction;

	if (m_acceptLanguagesAction)
		delete m_acceptLanguagesAction;
}

wchar_t* IEAction::GetName()
{
	return _getStringFromResourceIDName(IDS_IEACTION_NAME, szName);
}

wchar_t* IEAction::GetDescription()
{	
	return _getStringFromResourceIDName(IDS_IEACTION_DESCRIPTION, szDescription);
}

IELPIAction * IEAction::_getLPIAction()
{
	if (m_LPIAction == NULL)
	{
		m_LPIAction = new IELPIAction(m_OSVersion, m_runner, m_fileVersionInfo, m_downloadManager);
	}
	return m_LPIAction;
}

IEAcceptLanguagesAction * IEAction::_getAcceptLanguagesAction()
{
	if (m_acceptLanguagesAction == NULL)
	{
		m_acceptLanguagesAction = new IEAcceptLanguagesAction(m_registry, m_fileVersionInfo, m_OSVersion);
	}
	return m_acceptLanguagesAction;	
}

LPCWSTR IEAction::GetLicenseID()
{
	if (m_doLPIAction)
	{
		return _getLPIAction()->GetLicenseID();
	}
	else
	{
		return NULL;
	}
}

bool IEAction::IsNeed()
{
	bool bNeed;

	m_doAcceptLanguagesAction = _getAcceptLanguagesAction()->IsNeed();
	m_doLPIAction = _getLPIAction()->IsNeed();

	bNeed = m_doLPIAction || m_doAcceptLanguagesAction;

	g_log.Log(L"IEAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());	
	return bNeed;
}

void IEAction::SetStatus(ActionStatus value)
{
	Action::SetStatus(value);
	if (value == Selected || value == NotSelected)
	{
		if (_getLPIAction()->GetStatus() == Selected || _getLPIAction()->GetStatus() == NotSelected)
			_getLPIAction()->SetStatus(value);

		if (_getAcceptLanguagesAction()->GetStatus() == Selected || _getAcceptLanguagesAction()->GetStatus() == NotSelected)
			_getAcceptLanguagesAction()->SetStatus(value);
	}
}

bool IEAction::IsDownloadNeed()
{
	if (m_doLPIAction)
	{
		return _getLPIAction()->IsDownloadNeed();
	}
	else
	{
		return false;
	}
}

bool IEAction::Download(ProgressStatus progress, void *data)
{	
	return _getLPIAction()->Download(progress, data);
}

void IEAction::Execute()
{	
	if (m_doLPIAction)
	{
		_getLPIAction()->Execute();
	}
	SetStatus(InProgress);
}

ActionStatus IEAction::GetStatus()
{
	if (status == InProgress)
	{		
		
		if (m_doLPIAction && _getLPIAction()->GetStatus() == InProgress)
			return InProgress;

		if (m_doAcceptLanguagesAction)
		{
			_getAcceptLanguagesAction()->Execute();
		}

		if (m_doLPIAction && _getLPIAction()->GetStatus() != Successful ||
			m_doAcceptLanguagesAction && _getAcceptLanguagesAction()->GetStatus() != Successful)			
		{
			SetStatus(FinishedWithError);
		}
		else
		{
			SetStatus(Successful);
		}
		
		g_log.Log(L"IEAction::GetStatus is '%s'", status == Successful ? L"Successful" : L"FinishedWithError");
	}
	return status;
}

const wchar_t* IEAction::GetVersion()
{	
	return m_version.c_str();
}

void IEAction::Serialize(ostream* stream)
{
	_getAcceptLanguagesAction()->Serialize(stream);
	_getLPIAction()->Serialize(stream);	 
}

void IEAction::CheckPrerequirements(Action * action)
{	
	_getLPIAction()->CheckPrerequirements(action);
	_getAcceptLanguagesAction()->CheckPrerequirements(action);

	if (_getLPIAction()->GetStatus() == AlreadyApplied &&
		_getAcceptLanguagesAction()->GetStatus() == AlreadyApplied)
	{
			SetStatus(AlreadyApplied);
			return;
	}

	// We do not have a good way of communication when one subactions cannot be applied
	// but the other can
	if (_getLPIAction()->GetStatus() == CannotBeApplied &&
		_getAcceptLanguagesAction()->GetStatus() == AlreadyApplied)
	{
			SetStatus(AlreadyApplied);
			return;
	}

	if (_getLPIAction()->GetStatus() == CannotBeApplied)
	{
		wcscpy_s(szCannotBeApplied, _getLPIAction()->GetCannotNotBeApplied());		
	}
}