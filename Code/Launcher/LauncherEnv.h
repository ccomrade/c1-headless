/**
 * @file
 * @brief Global launcher environment.
 */

#pragma once

class Log;
class TaskSystem;

struct LauncherEnv
{
	Log *pLog;
	TaskSystem *pTaskSystem;

	int gameVersion;
	unsigned long mainThreadID;
};

extern LauncherEnv *gLauncher;

bool IsMainThread();

