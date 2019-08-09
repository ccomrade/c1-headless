/**
 * @file
 * @brief Global launcher environment.
 */

#pragma once

class TaskSystem;

struct LauncherEnv
{
	TaskSystem *pTaskSystem;

	int gameVersion;
	unsigned long mainThreadID;
};

extern LauncherEnv *gLauncher;

bool IsMainThread();

