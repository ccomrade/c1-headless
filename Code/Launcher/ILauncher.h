/**
 * @file
 * @brief Launcher API interface.
 */

#pragma once

#include <stddef.h>
#include <stdarg.h>

struct ILauncherTask;

struct ILauncher
{
	/**
	 * @brief Function exported by the launcher as "GetILauncher".
	 */
	typedef ILauncher *(*TGetFunc)();

	virtual const char *GetName() = 0;

	virtual int GetVersionMajor() = 0;
	virtual int GetVersionMinor() = 0;

	virtual int GetGameVersion() = 0;

	virtual int GetDefaultLogVerbosity() = 0;

	virtual unsigned long GetMainThreadID() = 0;

	/**
	 * @brief Adds new task to be executed in main thread.
	 * This function can be called from any thread.
	 * @param pTask The task allocated on heap using the "new" operator.
	 */
	virtual void DispatchTask( ILauncherTask *pTask ) = 0;

	virtual void LogToStdOut( const char *format, ... ) = 0;
	virtual void LogToStdErr( const char *format, ... ) = 0;

	virtual void LogToStdOutV( const char *format, va_list args, const char *prefix = NULL ) = 0;
	virtual void LogToStdErrV( const char *format, va_list args, const char *prefix = NULL ) = 0;
};

