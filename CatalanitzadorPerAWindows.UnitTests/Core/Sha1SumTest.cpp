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
#include "Defines.h"
#include "Sha1Sum.h"
#include <windows.h>

#include <fstream>

using ::testing::StrCaseEq;

void _createFile(wstring& file)
{
	char szBuff[] = "ce4b01c1d705f33204d352dfdfc2d7ab97134c9 *LIP_ca-ES-32bit.mlc";
	wchar_t szFile[MAX_PATH];

	GetTempPath(MAX_PATH, szFile);
	wcscat_s(szFile, L"test.sha1");
	file = szFile;

	ofstream of(szFile);
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();
}

// Creates a file a computes the sha1 (the file contains actually another sha1, but this is not rellevant)
TEST(Sha1SumTest, ComputeforFile)
{
	wstring file, computed;
	_createFile(file);

	Sha1Sum sha1sum(file);
	sha1sum.ComputeforFile();
	DeleteFile(file.c_str());

	EXPECT_THAT(sha1sum.GetSum(), StrCaseEq(L"bcb3c3873226eb348b25ad779acaefcd8b4bf0e9"));
}

// Creates a file that contains a SHA1 signature and reads it
TEST(Sha1SumTest, ReadFromFile)
{
	wstring file, computed;
	_createFile(file);

	Sha1Sum sha1sum(file);
	sha1sum.ReadFromFile();
	DeleteFile(file.c_str());

	EXPECT_THAT(sha1sum.GetSum(), StrCaseEq(L"ce4b01c1d705f33204d352dfdfc2d7ab97134c9"));
}
