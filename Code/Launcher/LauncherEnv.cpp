/**
 * @file
 * @brief Implementation of global launcher environment.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Launcher headers
#include "LauncherEnv.h"

LauncherEnv *gLauncher;

bool IsMainThread()
{
	return GetCurrentThreadId() == gLauncher->mainThreadID;
}

