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
	m_lastOccurrence = false;
	m_tailLines.resize(m_maxLinesTail);
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
			m_tailLines[i] = m_tailLines[i + 1];
		}
		m_tailLines[m_tailPos - 1] = line;
	}
	else
	{
		m_tailLines[m_tailPos] = line;
		m_tailPos++;
	}
}

FILE* LogExtractor::_openFile()
{
	FILE* stream;

	stream = _wfsopen(m_filename.c_str(), m_unicode ? L"rb" : L"r", _SH_DENYNO);

	if (stream == NULL)
	{
		g_log.Log(L"LogExtractor::_openFile. Error opening '%s'", (wchar_t*) m_filename.c_str());		
	}

	return stream;
}

void LogExtractor::ExtractLogFragmentForKeyword(wstring keyword)
{
	bool found = false;
	int headLines = 0;	
	FILE* stream;
	const int BUFFER_ELEMENTS = 16384;
	wchar_t szLine[BUFFER_ELEMENTS];
	
	m_lines.clear();
	m_keyword = keyword;
	std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

	stream = _openFile();
	if (stream == NULL)
	{
		return;
	}
	
	while(fgetws(szLine, BUFFER_ELEMENTS, stream))
	{
		if (found == false)
		{
			_storeTailLine(szLine);

			for (unsigned int i = 0; i < wcslen(szLine); i ++)
				szLine[i] = tolower(szLine[i]);

			if (wcsstr(szLine, keyword.c_str()) == 0)
				continue;

			m_lines.clear();

			for (unsigned int i = 0; i < m_tailLines.size(); i++)
			{
				m_lines.push_back(m_tailLines[i]);
			}
			
			found = true;
			continue;
		}
		else
		{
			_storeTailLine(szLine);
			m_lines.push_back(szLine);
			headLines++;
		}

		if (headLines >= m_maxLinesHead)
		{
			if (m_lastOccurrence == false)
			{
				break;
			}
			else
			{
				headLines = 0;
				found = false;
			}
		}
	}

	fclose(stream);

	if (m_lines.size() > 0)
	{
		_removeInvalidCharsInArray();
	}
	else
	{
		m_lines.clear();
	}
}

void LogExtractor::ExtractLines()
{
	int lines = 0;	
	FILE* stream;
	const int BUFFER_ELEMENTS = 16384;
	wchar_t szLine[BUFFER_ELEMENTS];

	m_lines.clear();
	
	stream = _openFile();
	if (stream == NULL)
	{
		return;
	}
	
	while(fgetws(szLine, BUFFER_ELEMENTS, stream))
	{
		m_lines.push_back(szLine);
		lines++;

		if (lines >= m_maxLinesHead + m_maxLinesTail)
		{
			break;
		}
	}

	fclose(stream);

	if (m_lines.size() > 0)
	{
		_removeInvalidCharsInArray();
	}	
}

void LogExtractor::DumpLines()
{
	g_log.Log(L"LogExtractor::DumpLines. For file '%s' and keyword '%s'", (wchar_t*) m_filename.c_str(), (wchar_t*) m_keyword.c_str());
	for (unsigned int i = 0; i < m_lines.size(); i++)
	{		
		g_log.Log(L"'%s'", (wchar_t*) m_lines[i].c_str());
	}		
}
