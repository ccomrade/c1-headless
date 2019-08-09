/**
 * @file
 * @brief Implementation of the log.
 */

#include <new>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Launcher headers
#include "Log.h"
#include "StringBuffer.h"
#include "ILauncherTask.h"
#include "LauncherEnv.h"
#include "TaskSystem.h"

typedef StringBuffer<2048> LogBuffer;

static void WriteToFile( HANDLE hFile, const LogBuffer & buffer )
{
	if ( ! hFile || hFile == INVALID_HANDLE_VALUE )
	{
		return;
	}

	LogBuffer tempBuffer;

	for ( size_t i = 0; i < buffer.getLength(); i++ )
	{
		if ( buffer[i] < 32 || buffer[i] == 127 )
		{
			// skip control characters
		}
		else
		{
			tempBuffer.append( buffer[i] );
		}
	}

	// add new line character
	tempBuffer.append( "\r\n" );  // CRLF

	DWORD bytesWritten;
	WriteFile( hFile, tempBuffer.get(), tempBuffer.getLength(), &bytesWritten, NULL );
}

struct WriteToFileTask : public ILauncherTask
{
	LogBuffer buffer;
	HANDLE hFile;

	WriteToFileTask()
	: buffer(),
	  hFile()
	{
	}

	void Run() override
	{
		WriteToFile( hFile, buffer );
	}
};

Log::Log()
{
}

Log::~Log()
{
}

void Log::LogToStdOut( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogToStdOutV( format, args, NULL );
	va_end( args );
}

void Log::LogToStdErr( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogToStdErrV( format, args, NULL );
	va_end( args );
}

void Log::LogToStdOutV( const char *format, va_list args, const char *prefix )
{
	if ( IsMainThread() )
	{
		LogBuffer buffer;
		buffer.append( prefix );
		buffer.append_vf( format, args );

		WriteToFile( GetStdHandle( STD_OUTPUT_HANDLE ), buffer );
	}
	else
	{
		WriteToFileTask *pTask = new WriteToFileTask();
		pTask->buffer.append( prefix );
		pTask->buffer.append_vf( format, args );
		pTask->hFile = GetStdHandle( STD_OUTPUT_HANDLE );

		gLauncher->pTaskSystem->AddTask( pTask );
	}
}

void Log::LogToStdErrV( const char *format, va_list args, const char *prefix )
{
	if ( IsMainThread() )
	{
		LogBuffer buffer;
		buffer.append( prefix );
		buffer.append_vf( format, args );

		WriteToFile( GetStdHandle( STD_ERROR_HANDLE ), buffer );
	}
	else
	{
		WriteToFileTask *pTask = new WriteToFileTask();
		pTask->buffer.append( prefix );
		pTask->buffer.append_vf( format, args );
		pTask->hFile = GetStdHandle( STD_ERROR_HANDLE );

		gLauncher->pTaskSystem->AddTask( pTask );
	}
}

