/**
 * @file
 * @brief Global launcher environment.
 */

#pragma once

struct LauncherEnv
{
	int gameVersion;
	unsigned long mainThreadID;
};

extern LauncherEnv *gLauncher;

bool IsMainThread();

