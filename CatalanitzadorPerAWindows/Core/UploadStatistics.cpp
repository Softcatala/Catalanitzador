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
#include "UploadStatistics.h"
#include "HttpFormInet.h"
#include <fstream>

UploadStatistics::UploadStatistics(ostream* stream)
{
	m_stream = stream;
}

UploadStatistics::~UploadStatistics()
{
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void UploadStatistics::UploadFile()
{
	BOOL result = 0;
	char szBuff[65535];
	char szVar[65535];
	
	streambuf* rdbuf = m_stream->rdbuf();
	memset (szBuff, 0, sizeof(szBuff));
	rdbuf->sgetn(szBuff, sizeof(szBuff));

	int n = strlen (szBuff);
	strcpy_s(szVar, "xml=");
	strcat_s(szVar, szBuff);

	// Dump XML to disc
	wchar_t szXML[MAX_PATH];
	GetTempPath(MAX_PATH, szXML);	
	wcscat_s(szXML, L"results.xml");

	ofstream of(szXML);
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();

	// Send file
	HttpFormInet access;
	bool rslt = access.PostForm(UPLOAD_URL, szVar);
#if !_DEBUG
	DeleteFile(szXML);
#endif
	g_log.Log (L"UploadStatistics::UploadFile result %u", (wchar_t *)rslt);	
}

DWORD UploadStatistics::_uploadXmlThead(LPVOID lpParam)
{
	UploadStatistics* stats = (UploadStatistics *) lpParam;
	stats->UploadFile();
	return 0;
}

void UploadStatistics::StartUploadThread()
{
	m_hThread = CreateThread(NULL, 0, _uploadXmlThead, this, 0, NULL);
}

void UploadStatistics::WaitBeforeExit()
{
	// In case the user exists the app very quickly, give time the upload to complete
	if (m_hThread != NULL)
		WaitForSingleObject(m_hThread, 10000);
}

