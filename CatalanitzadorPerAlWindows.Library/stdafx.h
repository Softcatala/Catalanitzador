// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// Cross cutting concerns

#include "Defines.h"
#include "version.h"

#include "resource.h"
#include "LogFile.h"
#include "Window.h"
#include "TriBool.h"
#include "RemoteURLs.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <Shlobj.h>
#include <Wincrypt.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <cstdio>


extern LogFile g_log;

