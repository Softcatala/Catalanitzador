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
#include "Thread.h"

using ::testing::StrCaseEq;

class ThreadTest : public Thread
{
public:

	ThreadTest(int value)
	{
		m_value = value;
	}

	int GetValue() const {return m_value;}

	void OnStart()
	{
		m_value++;
	}

private:

	int m_value;
};

TEST(ThreadTest, _performTask)
{
	int value = 1;
	ThreadTest threadTest(value);

	threadTest.Start();
	threadTest.Wait();

	EXPECT_THAT(threadTest.GetValue(), 2);	
}
