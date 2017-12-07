/*
Common definitions and macros for different projects
*/
#pragma once

#include <Windows.h>
#include <tchar.h>

// Calculates the length of a static array / string, in characters
#define LENGTHOF(arr) ((sizeof(arr))/(sizeof((arr)[0])))

// Safely closes a handle
#define CLOSE_HANDLE(hHandle) {if (NULL != (hHandle)) {(VOID)CloseHandle(hHandle); (hHandle) = NULL;}}

// Useful bases
#define BASE_DECIMAL (10)
#define BASE_HEXADECIMAL (16)

// Output data to user
#define OUTPUT(szFormat, ...) (VOID)_tprintf(_T(szFormat "\n"), __VA_ARGS__)
