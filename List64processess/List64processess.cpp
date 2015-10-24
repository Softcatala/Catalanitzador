// List64processess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>

// http://stackoverflow.com/questions/3801517/how-to-enum-modules-in-a-64bit-process-from-a-32bit-wow-process
void PrintRunningProcessesNames( wchar_t* filename)
{
	DWORD aProcesses[4096], cbNeeded;
	FILE* file;
	
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {	
		return;		
    }

	file = _wfopen(filename, L"w");

	if (file == NULL)
	{
		return;
	}
    
    for (unsigned int i = 0; i < cbNeeded / sizeof(DWORD); i++)
    {
		wchar_t szProcessName[MAX_PATH];

        if (aProcesses[i] != 0)
        {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

			if (hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(wchar_t));				
					fwprintf(file, szProcessName);
					fwprintf(file, L"\n");
				}
				CloseHandle(hProcess);
			}
        }
    }
	fclose(file);
}



int _tmain(int argc, wchar_t* argv[])
{
	if (argc < 2) return 1;
	PrintRunningProcessesNames(argv[1]);
	return 0;
}

