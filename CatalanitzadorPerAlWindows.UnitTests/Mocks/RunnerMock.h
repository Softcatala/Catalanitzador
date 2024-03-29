/* 
 * Copyright (C) 2012 Jordi Mas i Hern�ndez <jmas@softcatala.org>
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

#include "IRunner.h"

class RunnerMock : public IRunner
{
public:

		MOCK_METHOD3(Execute, bool(wchar_t*, wchar_t*, bool));
		MOCK_CONST_METHOD0(IsRunning, bool());
		MOCK_CONST_METHOD0(WaitUntilFinished, void());
		MOCK_CONST_METHOD1(GetProcessID, vector <DWORD>(wstring));
		MOCK_CONST_METHOD0(GetRunningProcessesNames, vector <wstring>());
		MOCK_METHOD1(RequestQuitToProcessID, bool(DWORD));
		MOCK_METHOD2(RequestCloseToProcessID, bool(DWORD, bool));
		MOCK_METHOD1(TerminateProcessID, bool(DWORD));
		MOCK_CONST_METHOD0(GetExitCode, DWORD());
};