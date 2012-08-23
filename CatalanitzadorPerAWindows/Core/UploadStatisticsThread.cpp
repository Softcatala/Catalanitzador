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

UploadStatisticsThread::UploadStatisticsThread(Serializer* serializer)
{
	m_serializer = serializer;
}


void UploadStatisticsThread::OnStart()
{
	string serialize;
	char szVar[65535];

	m_serializer->SaveToString(serialize);

	strcpy_s(szVar, "xml=");
	strcat_s(szVar, serialize.c_str());

	// Send file
	HttpFormInet access;	
	bool rslt = access.PostForm(UPLOAD_URL, szVar);
	g_log.Log(L"UploadStatisticsThread::UploadFile to %s, result %u", (wchar_t*) UPLOAD_URL, (wchar_t *)rslt);	
}

