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

Serializer::Serializer(wchar_t* file)
{
	stream = new ofstream (file);
	
	*stream << "<?xml version='1.0'?>\r\n";
	*stream << "<execution>\r\n";

	_application ();
	OSVersion::Serialize(stream);
}

Serializer::~Serializer()
{
	Close();
}

void Serializer::_application()
{	
	char szText [1024];

	sprintf_s (szText, "\t<application MajorVersion='%u' MinorVersion='%u' Revision='%u' />\r\n", 
		APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_REVISION);
	*stream << szText;
}

void Serializer::StartAction()
{
	*stream << "\t<actions>\r\n";
}

void Serializer::EndAction()
{
	*stream << "\t</actions>\r\n";
}

void Serializer::Serialize(Serializable* serializable)
{
	serializable->Serialize(stream);
}

void Serializer::Close()
{
	if (stream != NULL)
	{
		*stream << "</execution>\r\n";
		stream->close();
		delete stream;
		stream = NULL;
	}
}
