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
#include "Action.h"
#include "Resources.h"

#include <fstream>
#include <iostream>

Action::Action()
{
	status = NotSelected;
	szCannotBeApplied[0] = NULL;	
}

void Action::GetLicense(wstring &license)
{
	LPCWSTR id;
	
	id = GetLicenseID();

	if (id == NULL)
		return;

	Resources::LoadResourceToString(L"LICENSE_FILES", id, license);	
}

wchar_t* Action::_getStringFromResourceIDName(int nID, wchar_t* string)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	LoadString(hInstance, nID, string, MAX_LOADSTRING);
	return string;
}

void Action::Serialize(ostream* stream)
{
	char szText[1024];

	sprintf_s(szText, "\t\t<action id='%u' version='%s' result='%u'/>\n",
		GetID(), GetVersion(), status);

	*stream << szText;
}

Action* Action::AnotherActionDependsOnMe(vector <Action *> * allActions)
{
	for (unsigned int i = 0; i < allActions->size (); i++)
	{		
		Action* action = allActions->at(i);

		if (GetID() == action->DependsOn())
			return action;
	}
	return NULL;
}

bool Action::_getFile(DownloadID downloadID, wstring file, ProgressStatus progress, void *data)
{	
	return m_actionDownload.GetFile(downloadID, file, progress, data);
}