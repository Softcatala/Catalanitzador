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
#include "HttpFormInet.h"
#include "Url.h"
#include "StringConversion.h"

#include <vector>
using namespace std;

// Encodes a single variable form
void HttpFormInet::UrlFormEncode(vector <string> variables, vector <string> values, string& encoded)
{
	assert(variables.size() == values.size());

	encoded.erase();

	for (unsigned int v = 0; v < variables.size(); v++)
	{
		if (v > 0)
			encoded+= "&";

		encoded+= variables.at(v);
		encoded+= "=";

		unsigned char* pos = (unsigned char *) values.at(v).c_str();
		for (unsigned int i = 0; i < values.at(v).size(); i++, pos++)
		{
			if (*pos =='.' ||
				(*pos >='0' && *pos <='9') ||
				(*pos >='A' && *pos <='Z') ||
				(*pos >='a' && *pos <='z') ||
				(*pos == '\r' || *pos == '\n' || *pos == '\t'))
			{
				encoded += *pos;
			}
			else if (*pos == ' ')
			{
				encoded += '+';
			}
			else
			{
				char string[16];
				sprintf_s(string, "%%%X", *pos);
				encoded += string;
			}
		}
	}
}

// Variables are added in ANSI at the end of the request that's why is an ANSI string
bool HttpFormInet::PostForm(wstring _url, vector <string> variables, vector <string> values)
{
	HINTERNET hConnect, hRequest;
	const wchar_t hdrs[] = L"Content-Type: application/x-www-form-urlencoded\n\r";
	const wchar_t* accept[2]= {L"*/*", NULL};
	Url url(_url);
	
	hConnect = InternetConnect(m_hInternet, url.GetHostname(),
		INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);

	if (hConnect == NULL)
		return false;

	hRequest = HttpOpenRequest(hConnect, L"POST", url.GetPathAndFileName(),  NULL, NULL, accept,
	  INTERNET_FLAG_RELOAD |INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT, 0);

	if (hRequest == NULL)
	{
		InternetCloseHandle(hConnect);
		return false;
	}

	string encoded;

	UrlFormEncode(variables, values, encoded);
	HttpSendRequest(hRequest, hdrs, wcslen(hdrs), (LPVOID) encoded.c_str(), encoded.size());

	// Get result of the operation
	DWORD dwCode = 0;
	DWORD dwSize = sizeof(dwCode);
	HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | 
			HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL);
	
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	return dwCode == 200;
}