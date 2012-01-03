#pragma once

#define MAX_LOADSTRING 1024

#ifdef CATALANITZADOR_CREATE_SHARED_LIBRARY
	#define _APICALL __declspec(dllexport) 	
#else // Using shared lib
   #define _APICALL  __declspec(dllimport)    
#endif 

