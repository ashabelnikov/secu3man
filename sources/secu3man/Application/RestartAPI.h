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

#pragma once

// Command line switch for restarted application
#ifndef RA_CMDLINE_RESTART_PROCESS
 #define RA_CMDLINE_RESTART_PROCESS TEXT("--Restart")
#endif

// Mutex unique name
#ifndef RA_MUTEX_OTHER_RESTARTING
 #define RA_MUTEX_OTHER_RESTARTING TEXT("SECU3-MUTEX_NAME")
#endif

// Return TRUE if Process was restarted
BOOL RA_CheckProcessWasRestarted();

// Check process command line for restart switch
// Call this function to check that is restarted instance
BOOL RA_CheckForRestartProcessStart();

// Wait till previous instance of process finish
BOOL RA_WaitForPreviousProcessFinish();

// Call it when process finish
BOOL RA_DoRestartProcessFinish();

// Call this function when you need restart application
// After call you must close an active instance of your application
BOOL RA_ActivateRestartProcess();
