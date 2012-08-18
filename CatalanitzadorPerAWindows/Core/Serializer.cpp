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
#include "Serializer.h"
#include "OSVersion.h"
#include "Version.h"

Serializer::Serializer()
{
	m_stream = new stringstream();
}

Serializer::~Serializer()
{
	Close();
}

void Serializer::_application()
{	
	char szText [1024];

	sprintf_s (szText, "\t<application MajorVersion='%u' MinorVersion='%u' Revision='%u' />\n", 
		APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_REVISION);
	*m_stream << szText;
}

void Serializer::StartAction()
{
	*m_stream << "\t<actions>\n";
}

void Serializer::EndAction()
{
	*m_stream << "\t</actions>\n";	
}

void Serializer::OpenHeader()
{
	OSVersion version;

	*m_stream << "<?xml version='1.0'?>\n";
	*m_stream << "<execution>\n";

	_application();
	version.Serialize(m_stream);
}

void Serializer::CloseHeader()
{
	*m_stream << "</execution>\n";
}

void Serializer::Serialize(Serializable* serializable)
{
	serializable->Serialize(m_stream);
}

void Serializer::Close()
{
	if (m_stream != NULL)
	{
		delete m_stream;
		m_stream= NULL;
	}
}

void Serializer::SaveToString(string& string)
{
	char szBuff[65535];

	m_stream->seekg(0);
	streambuf* rdbuf = m_stream->rdbuf();
	memset(szBuff, 0, sizeof(szBuff));
	rdbuf->sgetn(szBuff, sizeof(szBuff));
	string = szBuff;	
}

void Serializer::SaveToFile(wstring file)
{
	string str;

	SaveToString(str);
	ofstream of((wchar_t* )file.c_str());
	of.write(str.c_str(), str.size());
	of.close();
}