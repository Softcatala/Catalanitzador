#pragma once

#define MAX_LOADSTRING 1024

#ifdef CATALANITZADOR_CREATE_SHARED_LIBRARY
	#define _APICALL __declspec(dllexport)
#else // Using shared lib
   #define _APICALL  __declspec(dllimport)
#endif 

// Disable STL warnings to reduce noise
#pragma warning(disable:4251)
