/*
 * Copyright (C) 2013 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "JavaRuntimeInspector.h"
#include "Java.h"
#include "OSVersion.h"

JavaRuntimeInspector::JavaRuntimeInspector(IRegistry* registry)
{
	m_registry = registry;
}

void JavaRuntimeInspector::Execute()
{
	_readVersion();
}

void JavaRuntimeInspector::_readVersion()
{
	OSVersion OSversion;
	Java java(&OSversion, m_registry, NULL);
	m_version = java.GetVersion();

	g_log.Log(L"JavaRuntimeInspector::_readVersion version %s", (wchar_t*) m_version.c_str());
	m_KeyValues.push_back(InspectorKeyValue(L"version", m_version.c_str()));
}

