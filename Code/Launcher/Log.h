/**
 * @file
 * @brief The log.
 */

#pragma once

#include <stddef.h>
#include <stdarg.h>

// CryEngine headers
#include "ILog.h"

class EngineLog : public ILog
{
	class Impl;
	Impl *m_impl;  // std::unique_ptr is C++11

public:
	EngineLog();
	~EngineLog();

	// --- IMiniLog ---

	void LogV( const ELogType type, const char *format, va_list args ) override;

	void Log( const char *format, ... ) override;
	void LogWarning( const char *format, ... ) override;
	void LogError( const char *format, ... ) override;

	// --- ILog ---

	void Release() override;

	bool SetFileName( const char *fileName ) override;
	const char *GetFileName() override;

	void LogPlus( const char *format, ... ) override;
	void LogToFile( const char *format, ... ) override;
	void LogToFilePlus( const char *format, ... ) override;
	void LogToConsole( const char *format, ... ) override;
	void LogToConsolePlus( const char *format, ... ) override;

	void UpdateLoadingScreen( const char *format, ... ) override;

	void RegisterConsoleVariables() override;
	void UnregisterConsoleVariables() override;

	void SetVerbosity( int verbosity ) override;
	int GetVerbosityLevel() override;

	void AddCallback( ILogCallback *pCallback ) override;
	void RemoveCallback( ILogCallback *pCallback ) override;
};

class Log
{
	EngineLog *m_pEngineLog;

public:
	Log();
	~Log();

	void LogToStdOut( const char *format, ... );
	void LogToStdErr( const char *format, ... );

	void LogToStdOutV( const char *format, va_list args, const char *prefix = NULL );
	void LogToStdErrV( const char *format, va_list args, const char *prefix = NULL );

	bool InitEngineLog();

	EngineLog *GetEngineLog()
	{
		return m_pEngineLog;
	}
};

