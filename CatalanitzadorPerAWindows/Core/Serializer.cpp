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
	stream = new stringstream();

	_openHeader();
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

	sprintf_s (szText, "\t<application MajorVersion='%u' MinorVersion='%u' Revision='%u' />\n", 
		APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_REVISION);
	*stream << szText;
}

void Serializer::StartAction()
{
	*stream << "\t<actions>\n";
}

void Serializer::EndAction()
{
	*stream << "\t</actions>\n";	
}

void Serializer::_openHeader()
{
	*stream << "<?xml version='1.0'?>\n";
	*stream << "<execution>\n";
}

void Serializer::CloseHeader()
{
	*stream << "</execution>\n";
}

void Serializer::Serialize(Serializable* serializable)
{
	serializable->Serialize(stream);
}

void Serializer::Close()
{
	if (stream != NULL)
	{
		delete stream;
		stream = NULL;
	}
}
