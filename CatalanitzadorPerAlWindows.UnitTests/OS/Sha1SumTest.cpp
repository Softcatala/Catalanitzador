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

using ::testing::StrCaseEq;

void _createFile(TempFile& file)
{
	char szBuff[] = "ce4b01c1d705f33204d352dfdfc2d7ab97134c9 *LIP_ca-ES-32bit.mlc";

	ofstream of(file.GetFileName().c_str());
	int size = strlen(szBuff);
	of.write(szBuff, size);
	of.close();
}

// Creates a file a computes the sha1 (the file contains actually another sha1, but this is not rellevant)
TEST(Sha1SumTest, ComputeforFile)
{
	wstring computed;
	TempFile file;

	_createFile(file);

	Sha1Sum sha1sum(file.GetFileName());
	sha1sum.ComputeforFile();

	EXPECT_THAT(sha1sum.GetSum(), StrCaseEq(L"bcb3c3873226eb348b25ad779acaefcd8b4bf0e9"));
}

// Creates a file that contains a SHA1 signature and reads it
TEST(Sha1SumTest, ReadFromFile)
{
	wstring computed;
	TempFile file;
	_createFile(file);

	Sha1Sum sha1sum(file.GetFileName());
	sha1sum.ReadFromFile();	

	EXPECT_THAT(sha1sum.GetSum(), StrCaseEq(L"ce4b01c1d705f33204d352dfdfc2d7ab97134c9"));
}

TEST(Sha1SumTest, EqualOperator)
{
	wstring computed;
	TempFile file;
	_createFile(file);

	Sha1Sum sha1(file.GetFileName());
	sha1.ReadFromFile();

	Sha1Sum sha2(file.GetFileName());
	sha2.ReadFromFile();
	
	EXPECT_TRUE(sha1==sha2);
}

TEST(Sha1SumTest, NotEqualOperator)
{
	wstring computed;
	TempFile file;
	_createFile(file);

	Sha1Sum sha1(file.GetFileName().c_str());
	sha1.ReadFromFile();

	Sha1Sum sha2(file.GetFileName().c_str());
	sha2.ComputeforFile();
	
	EXPECT_TRUE(sha1!=sha2);
}

TEST(Sha1SumTest, EmptyFile)
{
	wstring computed;
	TempFile file;

	ofstream of(file.GetFileName().c_str());
	of.close();

	Sha1Sum sha1(file.GetFileName().c_str());
	sha1.ReadFromFile();
	EXPECT_TRUE(sha1.GetSum().empty());
}

TEST(Sha1SumTest, SetFromString)
{
	const wchar_t * MD5 = L"ce4b01c1d705f33204d352dfdfc2d7ab97134c9";

	Sha1Sum sha1sum;
	sha1sum.SetFromString(MD5);

	EXPECT_THAT(sha1sum.GetSum(), StrCaseEq(MD5));
}