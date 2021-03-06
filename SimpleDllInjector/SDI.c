/*
Simple DLL Injector
Injects a given DLL to a target process, calling its DllMain entry point
Params: <Target PID> <DLL Path>
*/
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "..\Common\Common.h"


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

#define PERMISSIONS_VIRTUALALLOCEX (PROCESS_VM_OPERATION)
#define PERMISSIONS_WRITEPROCESSMEMORY (PROCESS_VM_WRITE | PROCESS_VM_OPERATION)
#define PERMISSIONS_CREATEREMOTETHREAD (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)
// CreateRemoteThread's permission contain the other permissions, but it's still more correct this way and more easily verifiable
#define REQUIRED_PERMISSIONS (PERMISSIONS_VIRTUALALLOCEX | PERMISSIONS_WRITEPROCESSMEMORY | PERMISSIONS_CREATEREMOTETHREAD)


INT
_tmain(
	_In_ INT	iArgc,
	_In_ PCTSTR	apszArgv[])
{
	SDI_STATUS eStatus = SDI_STATUS_INVALID_VALUE;
	HANDLE hProcess = NULL;
	DWORD dwPid = 0;
	TCHAR szDllPath[MAX_PATH] = _T("");
	SIZE_T cbDllPath = 0;
	HRESULT hrResult = E_FAIL;
	HMODULE hKernel32 = NULL;
	FARPROC pfnLoadLibrary = NULL;
	PVOID pvRemoteAddress = NULL;
	HANDLE hRemoteThread = NULL;
	
	if ((ARG_INDEX_COUNT != iArgc) || (NULL == apszArgv))
	{
		eStatus = SDI_STATUS_INVALID_ARGS;
		if (1 == iArgc)
		{
			OUTPUT("Usage: SDI.exe <Target PID> <DLL Path>");
		}
		else
		{
			OUTPUT("Bad args. iArgc=%d, apszArgv=0x%p", iArgc, apszArgv);
		}
		goto lblCleanup;
	}
	
	dwPid = _tcstoul(apszArgv[ARG_INDEX_PID], NULL, BASE_DECIMAL);
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

	// _tcsnlen will return at most LENGTHOF(szDllPath)-1, but probably less.
	// Add 1 for terminating-null
	cbDllPath = (_tcsnlen(szDllPath, LENGTHOF(szDllPath)-1) + 1) * sizeof(TCHAR);

	hProcess = OpenProcess(REQUIRED_PERMISSIONS, FALSE, dwPid);
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

	pvRemoteAddress = VirtualAllocEx(
		hProcess,
		NULL,
		cbDllPath,
		MEM_COMMIT|MEM_RESERVE,
		PAGE_READWRITE);
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
		cbDllPath,
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
