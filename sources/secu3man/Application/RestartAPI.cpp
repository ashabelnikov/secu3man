/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 by Alexander D. Alexeev
//   http://alex-boss.narod.ru
//   mailto:Alex.D.Alexeev@gmail.com
//
// You may do whatever you like with this file, I just don't care.
// If you find this code useful, credits would be nice!
// If you have any suggestions, welcome!
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RestartAPI.h"
#include "shlwapi.h"

// Global Variables
HANDLE g_RA_hMutexOtherRestarting = NULL;	// Mutex
BOOL g_RA_bWasRestarted = FALSE;			// Restarted Flag

BOOL RA_CheckProcessWasRestarted()
{
	return g_RA_bWasRestarted;
}

BOOL RA_CheckForRestartProcessStart()
{
	// Simple find substring in command line
	LPTSTR szCmdLine = ::GetCommandLine();
	return strstr(szCmdLine, RA_CMDLINE_RESTART_PROCESS) != NULL;
}

BOOL RA_WaitForPreviousProcessFinish()
{
	// App restarting
	BOOL AlreadyRunning;
	// Try to Create Mutex
	g_RA_hMutexOtherRestarting = ::CreateMutex( NULL, FALSE, RA_MUTEX_OTHER_RESTARTING);
	DWORD dwLastError = ::GetLastError();
	AlreadyRunning = (dwLastError == ERROR_ALREADY_EXISTS || dwLastError == ERROR_ACCESS_DENIED);
	if ( AlreadyRunning )
	{
		// Waiting for previous instance release mutex
		::WaitForSingleObject(g_RA_hMutexOtherRestarting, INFINITE);
		::ReleaseMutex(g_RA_hMutexOtherRestarting);
		g_RA_bWasRestarted = TRUE;
	}
	::CloseHandle(g_RA_hMutexOtherRestarting);
	g_RA_hMutexOtherRestarting = NULL;
	return TRUE;
}

BOOL RA_DoRestartProcessFinish()
{
	// Releasing mutex signal that process finished
	DWORD dwWaitResult = WaitForSingleObject(g_RA_hMutexOtherRestarting, 0);
	if (dwWaitResult == WAIT_TIMEOUT)
		::ReleaseMutex(g_RA_hMutexOtherRestarting);
	::CloseHandle(g_RA_hMutexOtherRestarting);
	g_RA_hMutexOtherRestarting = NULL;
	return (dwWaitResult == WAIT_TIMEOUT);
}
BOOL RA_ActivateRestartProcess()
{
	// Restart App
	BOOL AlreadyRunning;
	g_RA_hMutexOtherRestarting = ::CreateMutex( NULL, TRUE, RA_MUTEX_OTHER_RESTARTING);
	DWORD dwLastError = ::GetLastError();
	AlreadyRunning = (dwLastError == ERROR_ALREADY_EXISTS || dwLastError == ERROR_ACCESS_DENIED);
	if (AlreadyRunning)
	{
		::WaitForSingleObject(g_RA_hMutexOtherRestarting, INFINITE);
		::ReleaseMutex(g_RA_hMutexOtherRestarting);
		::CloseHandle(g_RA_hMutexOtherRestarting);
		return FALSE;
	}

	TCHAR szAppPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szAppPath, MAX_PATH);

	STARTUPINFO				si = {0};
	PROCESS_INFORMATION		pi = {0};
	si.cb = sizeof(STARTUPINFO);
	// Create New Instance command line
	::PathQuoteSpaces(szAppPath);
	::lstrcat(szAppPath, _T(" "));
	::lstrcat(szAppPath, RA_CMDLINE_RESTART_PROCESS); // Add command line key for restart
	// Create another copy of processS
	return ::CreateProcess(NULL, szAppPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
