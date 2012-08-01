/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
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
#include "guid.h"
#include "registry.h"
#include "StringConversion.h"

Guid::Guid(IRegistry* registry)
{
	m_bReaded = false;
	m_registry = registry;

	m_bReaded = _read();

	if (m_bReaded == false)
	{
		_generate();
	}
}

wstring Guid::Get()
{	
	return m_guid;
}

void Guid::Store()
{
	bool bRslt;

	assert(m_guid.size() > 0);

	// If the value was readed we do not to stored it again, since we will be writting again the same
	if (m_bReaded == true)
		return;

	bRslt = m_registry->CreateKey(HKEY_LOCAL_MACHINE, CATALANITZADOR_REGKEY);

	if (bRslt)
	{
		m_registry->SetString(GUID_REGKEY, (wchar_t*) m_guid.c_str());
		m_registry->Close();
	}

	g_log.Log(L"Guid::Store. Result '%u'", (wchar_t *) bRslt);
}

void Guid::_generate()
{
	GUID guid;
	RPC_WSTR pszUuid = 0;

	if (UuidCreate(&guid) == RPC_S_OK)
	{
		UuidToString(&guid, &pszUuid);
		m_guid = (wchar_t*) pszUuid;
		RpcStringFree(&pszUuid);
	}
	g_log.Log(L"Guid::_generate. Guid: '%s'", (wchar_t *) m_guid.c_str());
}

bool Guid::_read()
{
	bool bRslt = false;

	if (m_registry->OpenKey(HKEY_LOCAL_MACHINE, CATALANITZADOR_REGKEY, false))
	{
		wchar_t szGuid[2048];

		if (m_registry->GetString(GUID_REGKEY, szGuid, sizeof(szGuid)))
		{
			m_guid = szGuid;
			bRslt = !m_guid.empty();
		}
		m_registry->Close();
		
	}
	g_log.Log(L"Guid::_read. Result %u, guid: '%s'", (wchar_t*) bRslt, (wchar_t *) m_guid.c_str());
	return bRslt;
}

void Guid::Serialize(ostream* stream)
{	
	char szText [1024];	
	string guid_value;

	StringConversion::ToMultiByte(m_guid.c_str(), guid_value);
	sprintf_s (szText, "\t<session guid='%s' />\n", guid_value.c_str());
	*stream << szText;
}


