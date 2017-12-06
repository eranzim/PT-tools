#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

//TODO: Move to Common.h
#define DECIMAL (10)
#define LENGTHOF(arr) ((sizeof(arr))/(sizeof((arr)[0])))
#define OUTPUT(szFormat, ...) (VOID)_tprintf(_T(szFormat "\n"), __VA_ARGS__)
#define CLOSE_HANDLE(hHandle) {if (NULL != (hHandle)) {(VOID)CloseHandle(hHandle); (hHandle) = NULL;}}

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
	SDI_STATUS_OPENPROCESS_FAILED,
	SDI_STATUS_GETMODULEHANDLE_FAILED,
	SDI_STATUS_GETPROCADDRESS_FAILED,
	SDI_STATUS_VIRTUALALLOCEX_FAILED,
	SDI_STATUS_WRITEPROCESSMEMORY_FAILED,
	SDI_STATUS_CREATEREMOTETHREAD_FAILED,
	
	// Must be last:
	SDI_STATUS_COUNT
} SDI_STATUS, *PSDI_STATUS;

#define KERNEL32_MODULE_NAME (_T("Kernel32"))
#ifdef _UNICODE
#define LOADLIBRARY_NAME ("LoadLibraryW")
#else
#define LOADLIBRARY_NAME ("LoadLibraryA")
#endif
#define REMOTE_ALLOCATION_SIZE (MAX_PATH)


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
	HMODULE hKernel32 = NULL;
	FARPROC pfnLoadLibrary = NULL;
	PVOID pvRemoteAddress = NULL;
	HANDLE hRemoteThread = NULL;
	
	if ((ARG_INDEX_COUNT != iArgc) || (NULL == apszArgv))
	{
		eStatus = SDI_STATUS_INVALID_ARGS;
		OUTPUT("Bad args. iArgc=%d, apszArgv=0x%p", iArgc, apszArgv);
		goto lblCleanup;
	}
	
	dwPid = _tcstoul(apszArgv[ARG_INDEX_PID], NULL, DECIMAL);
	if ((0 == dwPid) || (ULONG_MAX == dwPid))
	{
		eStatus = SDI_STATUS_INVALID_PID;
		OUTPUT("_tcstoul failed with %lu. pid='%s', LE=%lu", dwPid, apszArgv[ARG_INDEX_PID], GetLastError());
		goto lblCleanup;
	}
	
	hrResult = StringCchCopy(szDllPath, LENGTHOF(szDllPath), apszArgv[ARG_INDEX_DLL_PATH]);
	if (FAILED(hrResult))
	{
		eStatus = SDI_STATUS_STRINGCCHCOPY_FAILED;
		OUTPUT("StringCchCopy failed with 0x%08X", hrResult);
		goto lblCleanup;
	}

	//TODO: Ask for less access
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
	{
		eStatus = SDI_STATUS_OPENPROCESS_FAILED;
		OUTPUT("OpenProcess failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	hKernel32 = GetModuleHandle(KERNEL32_MODULE_NAME);
	if (NULL == hKernel32)
	{
		eStatus = SDI_STATUS_GETMODULEHANDLE_FAILED;
		OUTPUT("GetModuleHandle failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	pfnLoadLibrary = GetProcAddress(hKernel32, LOADLIBRARY_NAME);
	if (NULL == pfnLoadLibrary)
	{
		eStatus = SDI_STATUS_GETPROCADDRESS_FAILED;
		OUTPUT("GetProcAddress failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	//TODO: Better to start with write / rw, then after write change to read-execute?
	//TODO: For size, is sizeof(szDllPath) enough? Should be.. Might even be enough to use its actual length * sizeof(TCHAR). Same goes for WriteProcessMemory.
	pvRemoteAddress = VirtualAllocEx(
		hProcess,
		NULL,
		REMOTE_ALLOCATION_SIZE,
		MEM_COMMIT|MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);
	if (NULL == pvRemoteAddress)
	{
		eStatus = SDI_STATUS_VIRTUALALLOCEX_FAILED;
		OUTPUT("VirtualAllocEx failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	if (!WriteProcessMemory(
		hProcess,
		pvRemoteAddress,
		szDllPath,
		sizeof(szDllPath),
		NULL))
	{
		eStatus = SDI_STATUS_WRITEPROCESSMEMORY_FAILED;
		OUTPUT("WriteProcessMemory failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	hRemoteThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(PTHREAD_START_ROUTINE)pfnLoadLibrary,
		pvRemoteAddress,
		0,
		NULL);
	if (NULL == hRemoteThread)
	{
		eStatus = SDI_STATUS_CREATEREMOTETHREAD_FAILED;
		OUTPUT("CreateRemoteThread failed. LE=%lu", GetLastError());
		goto lblCleanup;
	}

	// Great success!
	eStatus = SDI_STATUS_SUCCESS;

lblCleanup:
	CLOSE_HANDLE(hRemoteThread);
	// Leak the memory on success, free only on failures
	if ((NULL != pvRemoteAddress) && (SDI_STATUS_SUCCESS != eStatus))
	{
		(VOID)VirtualFreeEx(hProcess, pvRemoteAddress, 0, MEM_RELEASE);
		pvRemoteAddress = NULL;
	}
	CLOSE_HANDLE(hProcess);

	return (INT)eStatus;
}
