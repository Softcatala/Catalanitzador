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

Action::Action()
{
	status = NotSelected;
	szCannotBeApplied[0] = NULL;	
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

bool Action::_getAssociatedFileSha1Sum(wchar_t* URL, wchar_t* file, Sha1Sum &sha1sum)
{
	DownloadInet inetacccess;
	wstring sha1_file(file);
	wstring sha1_url(URL);

	sha1_file += SHA1_EXTESION;
	sha1_url += SHA1_EXTESION;

	if (inetacccess.GetFile((wchar_t *)sha1_url.c_str(), (wchar_t *)sha1_file.c_str(), NULL, NULL) == false)
	{		
		return false;
	}

	sha1sum.SetFile(sha1_file);
	sha1sum.ReadFromFile();
	DeleteFile(sha1_file.c_str());
	return sha1sum.GetSum().empty() == false;
}

bool Action::_getFile(wchar_t* URL, wchar_t* file, ProgressStatus progress, void *data)
{	
	DownloadInet inetacccess;
	Sha1Sum sha1_computed(file), sha1_read;

	// Get file
	if (inetacccess.GetFile(URL, file, progress, data) == false)
		return false;

	// If cannot get the file, cannot check assume that is OK
	if (_getAssociatedFileSha1Sum(URL, file, sha1_read) == false)
		return true;
	
	sha1_computed.ComputeforFile();
	return sha1_computed == sha1_read;
}