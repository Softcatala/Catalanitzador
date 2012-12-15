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
 
#pragma once

#include <string>
#include <vector>

using namespace std;

class LogExtractor
{
	public:

		LogExtractor(wstring filename, int maxLines);
		void ExtractLogFragmentForKeyword(wstring keyword);
		void ExtractLines();
		void DumpLines();
		const vector <wstring> & GetLines() { return m_lines; }
		void SetFileIsUnicode(bool unicode) { m_unicode = unicode; }
		void SetExtractLastOccurrence(bool lastOccurrence) { m_lastOccurrence = lastOccurrence; }

	private:

		void _storeTailLine(wstring line);
		void _removeInvalidChars(wstring& line);
		void _removeInvalidCharsInArray();
		FILE* _openFile();		

		vector <wstring> m_lines;
		vector <wstring> m_tailLines;
		int m_tailPos;
		int m_maxLinesHead;
		int m_maxLinesTail;
		wstring m_filename;
		wstring m_keyword;
		bool m_unicode;
		bool m_lastOccurrence;
};
