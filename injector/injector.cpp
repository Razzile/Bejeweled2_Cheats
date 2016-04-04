// injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <fstream>

HANDLE GetProcessByName(PCWSTR name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process)) {
		do {
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (lstrcmpiW(process.szExeFile, L"popcapgame1.exe") == 0)
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0) {
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	// Not found


	return NULL;
}

BOOL DLLExists(std::string path) 
{
	std::ifstream stream(path.data());
	return stream.good();
}

BOOL InjectLibrary(HANDLE hProcess, const char *fnDll)
{
	BOOL success = FALSE;
	HANDLE hThread = NULL;
	char *fnRemote = NULL;
	FARPROC procLoadLibraryA = NULL;

	size_t lenFilename = strlen(fnDll) + 1;

	/* Allocate space in the remote process */
	fnRemote = (char *)VirtualAllocEx(hProcess, NULL, lenFilename, MEM_COMMIT, PAGE_READWRITE);

	if (fnRemote) {
		/* Write the filename to the remote process. */
		if (WriteProcessMemory(hProcess, fnRemote, fnDll, lenFilename, NULL)) {
			/* Get the address of the LoadLibraryA function */
			procLoadLibraryA = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");
			hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)procLoadLibraryA, fnRemote, 0, NULL);
			if (hThread) {
				WaitForSingleObject(hThread, INFINITE);
				success = TRUE;
			}
		}
		VirtualFreeEx(hProcess, fnRemote, 0, MEM_RELEASE);
	}

	return success;
}

int main()
{
	char *buf = new char[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);

	std::string path(buf);
	path += "\\Bej2Hax.dll";

	if (DLLExists(path)) {
		if (!InjectLibrary(GetProcessByName(L"popcapgame1.exe"), path.c_str())) {
			printf("could not inject library\n");
		}
	}
	else {
		printf("oops\n");
	}
}

