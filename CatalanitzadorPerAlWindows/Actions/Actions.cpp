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

#include "stdafx.h"
#include <time.h>
#include "Actions.h"
#include "OSVersion.h"
#include "WindowsLPIAction.h"
#include "Windows8LPIAction.h"
#include "ConfigureLocaleAction.h"
#include "ConfigureDefaultLanguageAction.h"
#include "MSOfficeLPIAction.h"
#include "IEAction.h"
#include "ChromeAction.h"
#include "FirefoxAction.h"
#include "Win32I18N.h"
#include "Registry.h"
#include "OpenOfficeAction.h"
#include "AdobeReaderAction.h"
#include "WindowsLiveAction.h"
#include "FileVersionInfo.h"
#include "CatalanitzadorUpdateAction.h"
#include "iTunesAction.h"
#include "SkypeAction.h"
#include "LangToolLibreOfficeAction.h"
#include "LangToolFirefoxAction.h"
#include "LibreOffice.h"
#include "ApacheOpenOffice.h"

Actions::Actions(DownloadManager* downloadManager)
{
	m_pDownloadManager = downloadManager;
	OSVersion version;
	_buildListOfActions(&version);
}

Actions::Actions(DownloadManager* downloadManager, IOSVersion* pOSVersion)
{
	m_pDownloadManager = downloadManager;
	_buildListOfActions(pOSVersion);
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
		// Pointers on this array are free'd but if the object has a destructor
		// it is not called
		void* object = m_objectsToDelete.at(i);
		delete object;
	}

	if (m_ieLPIAction)
	{
		delete m_ieLPIAction;
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

IOpenOffice* Actions::_getLibreOffice(IRegistry* registry)
{
	IOpenOffice* openOffice;
	openOffice = (IOpenOffice *)new LibreOffice(registry);
	m_objectsToDelete.push_back(openOffice);
	return openOffice;
}

IOpenOffice* Actions::_getApacheOpenOffice(IRegistry* registry)
{
	IOpenOffice* openOffice;
	openOffice = (IOpenOffice *)new ApacheOpenOffice(registry);
	m_objectsToDelete.push_back(openOffice);
	return openOffice;
}

IEAcceptLanguagesAction* Actions::_getIEAcceptLanguagesAction()
{
	IEAcceptLanguagesAction* action;
	action = new IEAcceptLanguagesAction(_getNewRegistry(),  _getFileVersionInfo(), _getNewOSVersion());
	m_objectsToDelete.push_back(action);
	return action;
}

void Actions::_buildListOfActions(IOSVersion* pOSversion)
{
	m_ieLPIAction = new IELPIAction(_getNewOSVersion(), _getNewRunner(), _getFileVersionInfo(), m_pDownloadManager);

	if (pOSversion->GetVersion() == Windows8 || pOSversion->GetVersion() == Windows81 || pOSversion->GetVersion() == Windows10)
	{
		m_actions.push_back(new Windows8LPIAction(_getNewOSVersion(), _getNewRegistry(), _getNewWin32I18N(), _getNewRunner(), m_pDownloadManager));
	}
	else
	{
		m_actions.push_back(new WindowsLPIAction(_getNewOSVersion(), _getNewRegistry(), _getNewWin32I18N(), _getNewRunner(), m_pDownloadManager));
	}
	
	m_actions.push_back(new MSOfficeLPIAction(_getNewOSVersion(), _getNewRegistry(), _getNewWin32I18N(), _getNewRunner(), m_pDownloadManager));	
	m_actions.push_back(new IEAction(m_ieLPIAction, _getIEAcceptLanguagesAction()));
	m_actions.push_back(new ConfigureLocaleAction(_getNewOSVersion(), _getNewRegistry(), _getNewRunner()));
		
	if (pOSversion->GetVersion() != Windows8 && pOSversion->GetVersion() != Windows81 && pOSversion->GetVersion() != Windows10)
	{
		m_actions.push_back(new ConfigureDefaultLanguageAction(_getNewOSVersion(), _getNewRegistry(), _getNewRunner()));
	}

	m_actions.push_back(new ChromeAction( _getNewRegistry()));
	m_actions.push_back(new FirefoxAction( _getNewRegistry(), _getNewRunner(), m_pDownloadManager));
	m_actions.push_back(new LangToolFirefoxAction(_getNewRegistry(), _getNewRunner(), m_pDownloadManager));
	m_actions.push_back(new OpenOfficeAction( _getNewRegistry(), _getNewRunner(), m_pDownloadManager));

	IRegistry* langToolLibreOfficeRegistry = _getNewRegistry();
	m_actions.push_back(new LangToolLibreOfficeAction(langToolLibreOfficeRegistry, _getNewRunner(),
		_getLibreOffice(langToolLibreOfficeRegistry), _getApacheOpenOffice(langToolLibreOfficeRegistry), m_pDownloadManager));

	m_actions.push_back(new AdobeReaderAction( _getNewRegistry(), _getNewRunner(), m_pDownloadManager));
	m_actions.push_back(new CatalanitzadorUpdateAction(_getNewRunner(), m_pDownloadManager));
	m_actions.push_back(new WindowsLiveAction(_getNewRegistry(), _getNewRunner(), _getFileVersionInfo(), m_pDownloadManager));
	m_actions.push_back(new iTunesAction(_getNewRegistry(), _getFileVersionInfo()));
	m_actions.push_back(new SkypeAction(_getNewRegistry(), _getFileVersionInfo()));
}

Action * Actions::GetActionFromID(ActionID actionID)
{
	for (unsigned int i = 0; i < m_actions.size(); i++)
	{		
		if (m_actions.at(i)->GetID() == actionID)
			return m_actions.at(i);
	}
	return NULL;
}

void Actions::CheckPrerequirements()
{
	for (unsigned int i = 0; i < m_actions.size(); i++)
	{
		clock_t start = clock();
		Action* action = m_actions.at(i);
		action->CheckPrerequirements(NULL);
#if _DEBUG
		g_log.Log(L"Actions::CheckPrerequirements (time): action %s, time %u ms", (wchar_t *) action->GetName(), (wchar_t *) (clock() - start));
#endif
	}
}