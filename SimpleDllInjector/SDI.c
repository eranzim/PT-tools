#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

//TODO: Move to Common.h
#define DECIMAL (10)

typedef enum _ARG_INDEX
{
	ARG_INDEX_EXE_PATH = 0,
	ARG_INDEX_PID,
	ARG_INDEX_DLL_PATH,
	
	// Must be last:
	ARG_INDEX_COUNT
} ARG_INDEX, *PARG_INDEX;

typedef enum _SDI_STATUS
{
	SDI_STATUS_INVALID_VALUE = -1,
	SDI_STATUS_SUCCESS = 0,
	SDI_STATUS_INVALID_ARGS,
	SDI_STATUS_INVALID_PID,
	SDI_STATUS_STRINGCCHCOPY_FAILED,
	
	// Must be last:
	SDI_STATUS_COUNT
} SDI_STATUS, *PSDI_STATUS;

INT
_tmain(
	_In_ INT	iArgc,
	_In_ PCTSTR	apszArgv[])
{
	SDI_STATUS eStatus = SDI_STATUS_INVALID_VALUE;
	HANDLE hProcess = NULL;
	DWORD dwPid = 0;
	TCHAR szDllPath[MAX_PATH] = _T("");
	HRESULT hrResult = E_FAIL;
	/*HANDLE threadHandle;
	HMODULE dllHandle;
	DWORD processID;
	FARPROC loadLibraryAddress;
	LPVOID baseAddress;

	char args[] = "C:\\somedll.dll";

	printf("Enter process ID: ");
	std::cin >> processID;
	
	printf("PID=%d\n",processID);*/
	
	if ((ARG_INDEX_COUNT != iArgc) || (NULL == apszArgv))
	{
		eStatus = SDI_STATUS_INVALID_ARGS;
		goto lblCleanup;
	}
	
	dwPid = _tcstoul(apszArgv[SDI_STATUS_INVALID_PID], NULL, DECIMAL);
	if ((0 == dwPid) || (ULONG_MAX == dwPid))
	{
		eStatus = SDI_STATUS_INVALID_PID;
		goto lblCleanup;
	}
	
	hrResult = StringCchCopy(szDllPath, _lengthof(szDllPath), apszArgv[ARG_INDEX_DLL_PATH]);
	if (FAILED(hrResult))
	{
		eStatus = SDI_STATUS_STRINGCCHCOPY_FAILED;
		goto lblCleanup;
	}

	//TODO: Ask for less access
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
/*	if(hProcess == NULL)
	{
		printf("Error unable to open process. Error code: %d", GetLastError());
		std::cin.get();

		return 0;
	}

	printf("Process handle %d is ready",processID);

	dllHandle = GetModuleHandle(L"Kernel32");

	if(dllHandle == NULL)
	{
		printf("Error unable to allocate kernel32 handle..Error code: %d. Press any key to exit...",GetLastError());
	}

	printf("kernel32 handle is ready\n");


	loadLibraryAddress = GetProcAddress(dllHandle,"LoadLibraryA");

	if(loadLibraryAddress == NULL)
	{
		printf("Cannot get LoadLibraryA() address. Error code: %d. Press any key to exit",GetLastError());
		std::cin.get();

		return 0;
	}

	printf("LoadLibrary() address is ready\n");

	baseAddress = VirtualAllocEx(
		hProcess,
		NULL,
		256,
		MEM_COMMIT|MEM_RESERVE,
		PAGE_READWRITE);

	if(baseAddress == NULL)
	{
		printf("Error unable to alocate memmory in remote process. Error code: %d. Press any key to exit", GetLastError());
		std::cin.get();

		return 0;
	}

	printf("Memory allocation succeeded\n");

	BOOL isSucceeded = WriteProcessMemory(
		hProcess,
		baseAddress,
		args,
		sizeof(args)+1,
		NULL);

	if(isSucceeded == 0)
	{
		printf("Error unable to write memory . Error code: %d Press any key to exit...",GetLastError());
		std::cin.get();

		return 0;
	}

	printf("Argument has been written\n");


	threadHandle = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)loadLibraryAddress,
		baseAddress,
		NULL,
		0);

	if(threadHandle != NULL)
	{
		printf("Remote thread has been created\n");
	}

	std::cin.get();*/

lblCleanup:
	return 0;
}
