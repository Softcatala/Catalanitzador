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
#include "UploadStatisticsThread.h"
#include "HttpFormInet.h"

#define TEXT_FIELD_LEN 65535

UploadStatisticsThread::UploadStatisticsThread(IHttpFormInet* httpformInet, Serializer* serializer, bool sessionHadErrors)
{
	m_serializer = serializer;
	m_httpFormInet = httpformInet;
	m_sessionHadErrors = sessionHadErrors;
}

void UploadStatisticsThread::OnStart()
{
	string serialize, logfile;
	vector <string> variables;
	vector <string> values;

	m_serializer->SaveToString(serialize);

	variables.push_back("xml");
	values.push_back(serialize);

	if (m_sessionHadErrors)
	{
		wstring logfilew = g_log.GetContent();

		if (logfilew.size() > TEXT_FIELD_LEN)
			logfilew = logfilew.substr(logfilew.size() - TEXT_FIELD_LEN, TEXT_FIELD_LEN);

		StringConversion::ToMultiByte(logfilew, logfile);

		variables.push_back("log");
		values.push_back(logfile);
	}

	// Send file
	HttpFormInet access;
	bool rslt = m_httpFormInet->PostForm(UPLOAD_URL, variables, values);
	g_log.Log(L"UploadStatisticsThread::UploadFile to %s, result %u", (wchar_t*) UPLOAD_URL, (wchar_t *)rslt);
}

