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
#include "LogExtractor.h"

#include <algorithm>
using namespace std;

#define SEPARATOR L"."
#define SEPARATOR_LEN 1

LogExtractor::LogExtractor(wstring filename, int maxLines)
{
	m_filename = filename;
	m_tailPos = 0;	
	m_maxLinesHead = maxLines / 2;
	m_maxLinesTail = maxLines - m_maxLinesHead;
	m_unicode = true;
}

void LogExtractor::_removeInvalidChars(wstring& line)
{
	line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
}

void LogExtractor::_removeInvalidCharsInArray()
{
	wstring line;	
	for (unsigned int i = 0; i < m_lines.size(); i++)
	{		
		_removeInvalidChars(m_lines[i]);	
	}
}

void LogExtractor::_storeTailLine(wstring line)
{
	if (m_tailPos >= m_maxLinesTail)
	{
		for (int i = 0; i < m_tailPos - 1; i++)
		{
			m_lines[i] = m_lines[i + 1];
		}
		m_lines[m_tailPos - 1] = line;
	}
	else
	{
		m_lines[m_tailPos] = line;
		m_tailPos++;
	}
}

void LogExtractor::ExtractLogFragmentForKeyword(wstring keyword)
{

	bool found = false;
	int headLines = 0;	
	FILE* stream;
	const int BUFFER_ELEMENTS = 16384;
	wchar_t szLine[BUFFER_ELEMENTS];
	
	_wfopen_s(&stream, m_filename.c_str(), m_unicode ? L"rb" : L"r");

	if (stream == NULL)
	{
		g_log.Log(L"LogExtractor::ExtractLogFragmentForKeyword. Error opening '%s'", (wchar_t*) m_filename.c_str());
		return;
	}
	
	m_lines.clear();
	m_lines.resize(m_maxLinesTail);
	std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
	
	while(fgetws(szLine, BUFFER_ELEMENTS, stream))
	{
		if (found == false)
		{
			_storeTailLine(szLine);

			for (unsigned int i = 0; i < wcslen(szLine); i ++)
				szLine[i] = tolower(szLine[i]);

			if (wcsstr(szLine, keyword.c_str()) == 0)			
				continue;			
			
			found = true;
			continue;
		}
		else
		{
			m_lines.push_back(szLine);
			headLines++;
		}

		if (headLines >= m_maxLinesHead)
			break;
	}

	fclose(stream);

	if (found == true)
	{
		_removeInvalidCharsInArray();
	}
	else
	{
		m_lines.clear();
	}
}

void LogExtractor::DumpLines()
{
	g_log.Log(L"LogExtractor::DumpLines. Dumping '%s'", (wchar_t*) m_filename.c_str());
	for (unsigned int i = 0; i < m_lines.size(); i++)
	{		
		g_log.Log(L"'%s'", (wchar_t*) m_lines[i].c_str());
	}		
}
