/**
 * @file
 * @brief Global launcher environment.
 */

#pragma once

class Log;
class TaskSystem;
class EngineListener;

struct ISystem;

struct LauncherEnv
{
	Log *pLog;
	TaskSystem *pTaskSystem;
	EngineListener *pEngineListener;

	ISystem *pSystem;

	int gameVersion;
	unsigned long mainThreadID;
};

extern LauncherEnv *gLauncher;

bool IsMainThread();

