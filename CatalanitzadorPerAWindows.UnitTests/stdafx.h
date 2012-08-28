// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>

#include "RegistryMock.h"
#include "RunnerMock.h"
#include "OSVersionMock.h"
#include "Win32I18NMock.h"
#include "FileVersionInfoMock.h"
#include "TempFile.h"
#include "LogFile.h"

LogFile g_log;

// TODO: reference additional headers your program requires here
