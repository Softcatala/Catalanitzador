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
#include "FirefoxAcceptLanguagesAction.h"

FirefoxAcceptLanguagesAction::FirefoxAcceptLanguagesAction(wstring profileRootDir, wstring locale, wstring version) 
: Action(), m_acceptLanguages(profileRootDir, locale)
{
	m_version = version;
}

bool FirefoxAcceptLanguagesAction::IsNeed()
{
	bool bNeed;

	switch(GetStatus())
	{		
		case NotInstalled:
		case AlreadyApplied:
		case CannotBeApplied:
			bNeed = false;
			break;
		default:
			bNeed = true;
			break;
	}

	g_log.Log(L"FirefoxAcceptLanguagesAction::IsNeed returns %u (status %u)", (wchar_t *) bNeed, (wchar_t*) GetStatus());	
	return bNeed;
}

void FirefoxAcceptLanguagesAction::Execute()
{	
	SetStatus(InProgress);
	m_acceptLanguages.Execute();
	
	if (_isAcceptLanguageOk())
	{
		SetStatus(Successful);
	}
	else
	{
		SetStatus(FinishedWithError);
	}
}

bool FirefoxAcceptLanguagesAction::_isAcceptLanguageOk()
{
	bool isOk = false;

	if (m_acceptLanguages.ReadLanguageCode())
	{
		if (m_acceptLanguages.IsNeed() == false)
		{			
			isOk = true;
		}
	}

	g_log.Log(L"FirefoxAcceptLanguagesAction::_isAcceptLanguageOk: %u", (wchar_t*) isOk);
	return isOk;	
}

void FirefoxAcceptLanguagesAction::CheckPrerequirements(Action * action)
{
	if (m_acceptLanguages.ReadLanguageCode() == false)
	{
		SetStatus(CannotBeApplied);
		return;
	}
	
	if (_isAcceptLanguageOk())
	{
		SetStatus(AlreadyApplied);
		return;
	}
}