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
#include "FinishPropertyPageUI.h"
#include "PropertySheetUI.h"
#include <fstream>

FinishPropertyPageUI::FinishPropertyPageUI()
{
	m_hThread = NULL;
}

FinishPropertyPageUI::~FinishPropertyPageUI()
{
	if (m_hThread == NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void FinishPropertyPageUI::_onInitDialog()
{
	m_hThread = CreateThread(NULL, 0, _uploadXmlThead, this, 0, NULL); 
}

DWORD FinishPropertyPageUI::_uploadXmlThead(LPVOID lpParam)
{
	BOOL result = 0;
	char szBuff[65535];
	char szVar[65535];
	FinishPropertyPageUI* pThis = (FinishPropertyPageUI*) lpParam;

	ostream* stream = pThis->m_serializer->GetStream();
	streambuf* rdbuf = stream->rdbuf();
	memset (szBuff, 0, sizeof(szBuff));
	rdbuf->sgetn(szBuff, sizeof(szBuff));

	int n = strlen (szBuff);
	strcpy(szVar, "xml=");
	strcat(szVar, szBuff);

	// Dump XML to disc
	wchar_t szXML[MAX_PATH];
	GetTempPath(MAX_PATH, szXML);	
	wcscat_s(szXML, L"results.xml");

	ofstream of (szXML);
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();

	// Send file
	InternetAccess access;
	bool rslt = access.PostForm(L"http://www.softvalencia.org/catalanitzador/parser.php", szVar);

	g_log.Log (L"FinishPropertyPageUI::_uploadXmlThead result %u", (wchar_t *)rslt);
	return 0;
}

bool FinishPropertyPageUI::_isRebootNeed()
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

void FinishPropertyPageUI::_onFinish()
{
	if (_isRebootNeed())
	{
		bool result;
		wchar_t szMessage [MAX_LOADSTRING];
		wchar_t szCaption [MAX_LOADSTRING];

		LoadString(GetModuleHandle(NULL), IDS_REBOOTNEED, szMessage, MAX_LOADSTRING);
		LoadString(GetModuleHandle(NULL), IDS_MSGBOX_CAPTION, szCaption, MAX_LOADSTRING);

		result = (MessageBox(getHandle(), szMessage, szCaption,
			MB_YESNOCANCEL | MB_ICONQUESTION) == IDYES);

		if (result)
		{
			ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_MAJOR_APPLICATION);
		}
	}

	// In case the user exists the app very quickly, give time the upload to complete
	if (m_hThread != NULL)
		WaitForSingleObject(m_hThread, 10000);

}