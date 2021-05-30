// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Cross cutting concerns


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <Shlobj.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <cstdio>
#include <iostream>

#include "Defines.h"
#include "version.h"

#include "resource.h"
#include "LogFile.h"
#include "Window.h"
#include "TriBool.h"
#include "RemoteURLs.h"


extern LogFile g_log;

