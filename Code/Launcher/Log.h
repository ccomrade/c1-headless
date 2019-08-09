/**
 * @file
 * @brief The log.
 */

#pragma once

#include <stddef.h>
#include <stdarg.h>

class Log
{
public:
	Log();
	~Log();

	void LogToStdOut( const char *format, ... );
	void LogToStdErr( const char *format, ... );

	void LogToStdOutV( const char *format, va_list args, const char *prefix = NULL );
	void LogToStdErrV( const char *format, va_list args, const char *prefix = NULL );
};

