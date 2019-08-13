/**
 * @file
 * @brief Global launcher environment.
 */

#pragma once

class Log;
class TaskSystem;
class Validator;
class EngineListener;

struct ISystem;

struct LauncherEnv
{
	Log *pLog;
	TaskSystem *pTaskSystem;
	Validator *pValidator;
	EngineListener *pEngineListener;

	ISystem *pSystem;

	int gameVersion;
	int defaultLogVerbosity;
	unsigned long mainThreadID;
	const char *rootFolder;
};

extern LauncherEnv *gLauncher;

bool IsMainThread();

