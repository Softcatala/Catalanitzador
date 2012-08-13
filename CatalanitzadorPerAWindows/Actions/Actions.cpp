/* 
 * Copyright (C) 2011 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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
#include "Actions.h"
#include "WindowsLPIAction.h"
#include "IEAcceptLanguagesAction.h"
#include "ConfigureLocaleAction.h"
#include "ConfigureDefaultLanguageAction.h"
#include "MSOfficeLPIAction.h"
#include "IELPIAction.h"
#include "ChromeAction.h"
#include "FirefoxAction.h"
#include "Win32I18N.h"
#include "Registry.h"
#include "OpenOfficeAction.h"
#include "AdobeReaderAction.h"
#include "WindowsLiveAction.h"
#include "FileVersionInfo.h"
#include "CatalanitzadorUpdateAction.h"

Actions::Actions(DownloadManager* downloadManager)
{
	m_pDownloadManager = downloadManager;
	_buildListOfActions();
}

Actions::~Actions()
{
	for (unsigned int i = 0; i < m_actions.size(); i++)
	{		
		Action* action = m_actions.at(i);
		delete action;
	}
	
	for (unsigned int i = 0; i < m_objectsToDelete.size(); i++)
	{
		void* object = m_objectsToDelete.at(i);
		delete object;
	}
}

IOSVersion* Actions::_getNewOSVersion()
{
	IOSVersion* version;
	version = (IOSVersion *)new OSVersion();
	m_objectsToDelete.push_back(version);
	return version;
}

IRegistry* Actions::_getNewRegistry()
{
	IRegistry* registry;
	registry = (IRegistry *)new Registry();
	m_objectsToDelete.push_back(registry);
	return registry;
}

IRunner* Actions::_getNewRunner()
{
	IRunner* runner;
	runner = (IRunner *)new Runner();
	m_objectsToDelete.push_back(runner);
	return runner;
}

IWin32I18N* Actions::_getNewWin32I18N()
{
	IWin32I18N* win32I18N;
	win32I18N = (IWin32I18N *)new Win32I18N();
	m_objectsToDelete.push_back(win32I18N);
	return win32I18N;
}

IFileVersionInfo* Actions::_getFileVersionInfo()
{
	IFileVersionInfo* fileVersionInfo;
	fileVersionInfo = (IFileVersionInfo *)new FileVersionInfo();
	m_objectsToDelete.push_back(fileVersionInfo);
	return fileVersionInfo;
}

void Actions::_buildListOfActions()
{
	m_actions.push_back(new WindowsLPIAction(_getNewOSVersion(), _getNewRegistry(), _getNewWin32I18N(), _getNewRunner()));
	m_actions.push_back(new MSOfficeLPIAction( _getNewRegistry(), _getNewRunner()));
	m_actions.push_back(new WindowsLiveAction( _getNewRegistry(), _getNewRunner(), _getFileVersionInfo()));
	m_actions.push_back(new IELPIAction(_getNewOSVersion(), _getNewRegistry(), _getNewRunner()));
	m_actions.push_back(new IEAcceptLanguagesAction( _getNewRegistry()));
	m_actions.push_back(new ConfigureLocaleAction());
	m_actions.push_back(new ConfigureDefaultLanguageAction(_getNewOSVersion(), _getNewRegistry(), _getNewRunner()));
	m_actions.push_back(new ChromeAction( _getNewRegistry()));
	m_actions.push_back(new FirefoxAction( _getNewRegistry()));
	m_actions.push_back(new OpenOfficeAction( _getNewRegistry(), _getNewRunner(), m_pDownloadManager));
	m_actions.push_back(new AdobeReaderAction( _getNewRegistry(), _getNewRunner()));
	m_actions.push_back(new CatalanitzadorUpdateAction(_getNewRunner(), m_pDownloadManager));

	_checkPrerequirements();
}

Action * Actions::GetActionFromID(ActionID actionID)
{
	for (unsigned int i = 0; i < m_actions.size(); i++)
	{		
		if (m_actions.at(i)->GetID() == actionID)
			return m_actions.at(i);
	}
	assert(false);
	return NULL;
}

void Actions::_checkPrerequirements()
{
	for (unsigned int i = 0; i < m_actions.size(); i++)
	{		
		Action* action = m_actions.at(i);
		action->CheckPrerequirements(NULL);
	}
}