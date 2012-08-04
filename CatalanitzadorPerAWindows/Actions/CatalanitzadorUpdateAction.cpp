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
#include <stdio.h>

#include "CatalanitzadorUpdateAction.h"
#include "Runner.h"
#include "ConfigurationInstance.h"
#include "Runner.h"

#define FILENAME L"CatalanitzadorPerAlWindows.exe"

CatalanitzadorUpdateAction::CatalanitzadorUpdateAction(IRunner* runner)
{
	m_runner = runner;
	SetVersion(ConfigurationInstance::Get().GetVersion().GetString());
}

wchar_t* CatalanitzadorUpdateAction::GetName()
{
	return L"";
}

wchar_t* CatalanitzadorUpdateAction::GetDescription()
{
	return L"";
}

bool CatalanitzadorUpdateAction::IsNeed()
{
 	return true;
}

bool CatalanitzadorUpdateAction::Download(ProgressStatus progress, void *data)
{
	DownloadInet downloadInet;
	Configuration configuration = ConfigurationInstance::Get();
	wstring surl;

	// TODO: Implement re-try for other mirrors
	surl = configuration.GetRemote().GetLatest().GetUrls().at(0);

	wchar_t szFilename[MAX_PATH];
	GetTempPath(MAX_PATH, szFilename);
	wcscat_s(szFilename, FILENAME);
	m_filename = szFilename;

	downloadInet.GetFile((wchar_t *)surl.c_str(), (wchar_t *)m_filename.c_str(), progress, data);
	return true;
}

#define PARAMETER_NOCHECK L""

void CatalanitzadorUpdateAction::Execute()
{
	wstring parameter(PARAMETER_NOCHECK);

	parameter += GetVersion();
	m_runner->Execute((wchar_t *)m_filename.c_str(), (wchar_t *)parameter.c_str(), false);
}
