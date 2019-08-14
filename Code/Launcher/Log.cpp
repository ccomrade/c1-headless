/**
 * @file
 * @brief Implementation of the log.
 */

#include <string.h>
#include <time.h>
#include <new>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// CryEngine headers
#include "ISystem.h"
#include "IConsole.h"
#include "ITimer.h"
//#include "Cry_Geo.h"  // required by IRenderer.h
//#include "IRenderer.h"

// Launcher headers
#include "Log.h"
#include "StringBuffer.h"
#include "ILauncherTask.h"
#include "LauncherEnv.h"
#include "TaskSystem.h"
#include "CmdLine.h"

#define LOG_DEFAULT_FILE_NAME "Server.log"
#define LOG_DEFAULT_VERBOSITY 1

typedef StringBuffer<2048> LogBuffer;

namespace ELogFlags
{
	enum
	{
		FILE    = (1 << 0),  //!< Log message to file.
		CONSOLE = (1 << 1),  //!< Log message to console.

		APPEND  = (1 << 2)   //!< Append message to the last line.
	};
}

class EngineLog::Impl
{
	ICVar *m_pLogVerbosityCVar;
	ICVar *m_pLogFileVerbosityCVar;
	ICVar *m_pLogIncludeTimeCVar;
	CTimeValue m_includeTimeStartTime;
	CTimeValue m_includeTimeLastTime;
	HANDLE m_hLogFile;
	std::string m_logFileName;
	std::vector<ILogCallback*> m_callbacks;

	void DoLog( const LogBuffer & buffer, int flags );
	void WriteToLogFile( const LogBuffer & buffer, int flags );
	void WriteToConsole( const LogBuffer & buffer, int flags );

	static void OnIncludeTimeValueChanged( ICVar *pCVar );

	struct LogTask : public ILauncherTask
	{
		Impl *pContext;
		LogBuffer buffer;
		int flags;

		LogTask( Impl *pThis )
		: pContext(pThis),
		  buffer(),
		  flags()
		{
		}

		void Run() override
		{
			pContext->DoLog( buffer, flags );
		}
	};

public:
	Impl()
	: m_pLogVerbosityCVar(NULL),
	  m_pLogFileVerbosityCVar(NULL),
	  m_pLogIncludeTimeCVar(NULL),
	  m_includeTimeStartTime(),
	  m_includeTimeLastTime(),
	  m_hLogFile(NULL),
	  m_logFileName(),
	  m_callbacks()
	{
	}

	~Impl()
	{
		if ( m_hLogFile )
		{
			CloseHandle( m_hLogFile );
		}

		UnregisterConsoleVariables();
	}

	const std::string & GetLogFileName() const
	{
		return m_logFileName;
	}

	int GetVerbosity() const
	{
		return (m_pLogVerbosityCVar) ? m_pLogVerbosityCVar->GetIVal() : gLauncher->defaultLogVerbosity;
	}

	int GetFileVerbosity() const
	{
		return (m_pLogFileVerbosityCVar) ? m_pLogFileVerbosityCVar->GetIVal() : gLauncher->defaultLogVerbosity;
	}

	void SetVerbosity( int verbosity )
	{
		if ( m_pLogVerbosityCVar )
		{
			m_pLogVerbosityCVar->Set( verbosity );
		}
	}

	void RegisterConsoleVariables();
	void UnregisterConsoleVariables();

	void AddCallback( ILogCallback *pCallback );
	void RemoveCallback( ILogCallback *pCallback );

	bool OpenLogFile( const char *fileName );

	void Log( ILog::ELogType msgType, const char *format, va_list args, int flags );
};

void EngineLog::Impl::DoLog( const LogBuffer & buffer, int flags )
{
	if ( flags & ELogFlags::FILE )
	{
		WriteToLogFile( buffer, flags );
	}

	if ( flags & ELogFlags::CONSOLE )
	{
		WriteToConsole( buffer, flags );
	}
}

static void AddCurrentTime( LogBuffer & buffer )
{
	char timeBuffer[16];
	time_t seconds = time( NULL );
	tm *currentTime = localtime( &seconds );  // this must be called only from main thread
	strftime( timeBuffer, sizeof timeBuffer, "%H:%M:%S", currentTime );

	buffer.append( timeBuffer );
}

static void AddRelativeTime( LogBuffer & buffer, CTimeValue & lastTime, bool updateLastTime )
{
	CTimeValue currentTime = gLauncher->pSystem->GetITimer()->GetAsyncTime();
	CTimeValue relativeTime = currentTime - lastTime;

	if ( updateLastTime )
	{
		lastTime = currentTime;
	}

	buffer.append_f( "%7.3f", relativeTime.GetSeconds() );
}

void EngineLog::Impl::WriteToLogFile( const LogBuffer & buffer, int flags )
{
	if ( ! m_hLogFile )
	{
		return;
	}

	int includeTime = 0;
	if ( m_pLogIncludeTimeCVar )
	{
		includeTime = m_pLogIncludeTimeCVar->GetIVal();
	}

	LogBuffer tempBuffer;

	if ( includeTime > 0 && gLauncher->pSystem )
	{
		switch ( includeTime )
		{
			case 1:
			{
				tempBuffer.append( '<' );
				AddCurrentTime( tempBuffer );
				tempBuffer.append( '>' );
				break;
			}
			case 2:
			{
				tempBuffer.append( '<' );
				AddRelativeTime( tempBuffer, m_includeTimeLastTime, true );
				tempBuffer.append( '>' );
				break;
			}
			case 3:
			{
				tempBuffer.append( '<' );
				AddCurrentTime( tempBuffer );
				tempBuffer.append( '>' );
				tempBuffer.append( ' ' );
				tempBuffer.append( '[' );
				AddRelativeTime( tempBuffer, m_includeTimeLastTime, true );
				tempBuffer.append( ']' );
				break;
			}
			case 4:
			{
				tempBuffer.append( '<' );
				AddRelativeTime( tempBuffer, m_includeTimeStartTime, false );
				tempBuffer.append( '>' );
				break;
			}
		}

		tempBuffer.append( ": " );
	}

	for ( size_t i = 0; i < buffer.getLength(); i++ )
	{
		if ( buffer[i] < 32 || buffer[i] == 127 )
		{
			// skip control characters
		}
		else if ( buffer[i] == '$' )
		{
			// convert "$$" to "$"
			if ( buffer[i+1] == '$' )
			{
				tempBuffer.append( '$' );
			}

			// skip color codes
			i++;
		}
		else
		{
			tempBuffer.append( buffer[i] );
		}
	}

	// add new line character
	tempBuffer.append( "\r\n" );  // CRLF

	// workaround for broken messages from CPU detection
	if ( buffer.getLength() > 2 && buffer[buffer.getLength()-2] == '\n' && buffer[buffer.getLength()-1] == '\n' )
	{
		// add one more new line if the message ends with 2 new line characters
		tempBuffer.append( "\r\n" );
	}

	const bool isNewLine = ! (flags & ELogFlags::APPEND);

	if ( ! isNewLine )
	{
		// move file pointer before the last new line character
		SetFilePointer( m_hLogFile, -2, NULL, FILE_END );  // CRLF is 2 bytes long
	}

	DWORD bytesWritten;  // required
	WriteFile( m_hLogFile, tempBuffer.get(), tempBuffer.getLength(), &bytesWritten, NULL );

	for ( std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it )
	{
		(*it)->OnWriteToFile( buffer.get(), isNewLine );
	}
}

void EngineLog::Impl::WriteToConsole( const LogBuffer & buffer, int flags )
{
	if ( ! gLauncher->pSystem )
	{
		return;
	}

	IConsole *pConsole = gLauncher->pSystem->GetIConsole();
	const bool isNewLine = ! (flags & ELogFlags::APPEND);

	if ( isNewLine )
	{
		pConsole->PrintLine( buffer.get() );
	}
	else
	{
		pConsole->PrintLinePlus( buffer.get() );
	}

	for ( std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it )
	{
		(*it)->OnWriteToConsole( buffer.get(), isNewLine );
	}
}

void EngineLog::Impl::OnIncludeTimeValueChanged( ICVar *pCVar )  // static function
{
	if ( ! gLauncher->pSystem )
	{
		return;
	}

	Impl *self = gLauncher->pLog->GetEngineLog()->m_impl;

	CTimeValue currentTime = gLauncher->pSystem->GetITimer()->GetAsyncTime();

	self->m_includeTimeLastTime = currentTime;
	self->m_includeTimeStartTime = currentTime;
}

void EngineLog::Impl::RegisterConsoleVariables()
{
	IConsole *pConsole = gLauncher->pSystem->GetIConsole();
	const int defaultVerbosity = gLauncher->defaultLogVerbosity;

	m_pLogVerbosityCVar = pConsole->RegisterInt( "log_Verbosity", defaultVerbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for console log messages (use log_FileVerbosity for file logging).\n"
	  "Usage: log_Verbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"  // CE5 feature
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_pLogFileVerbosityCVar = pConsole->RegisterInt( "log_FileVerbosity", defaultVerbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for file log messages (if log_Verbosity defines higher value, this one is used).\n"
	  "Usage: log_FileVerbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"  // CE5 feature
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_pLogIncludeTimeCVar = pConsole->RegisterInt( "log_IncludeTime", 0, VF_NOT_NET_SYNCED,
	  "Toggles time stamping of log entries.\n"
	  "Usage: log_IncludeTime [0/1/2/3/4]\n"
	  "  0 = Off (default).\n"
	  "  1 = Current time.\n"
	  "  2 = Relative time.\n"
	  "  3 = Current + Relative time.\n"
	  "  4 = Absolute time in seconds since this mode was started.",
	  OnIncludeTimeValueChanged
	);
}

void EngineLog::Impl::UnregisterConsoleVariables()
{
	m_pLogVerbosityCVar = NULL;
	m_pLogFileVerbosityCVar = NULL;
	m_pLogIncludeTimeCVar = NULL;
}

void EngineLog::Impl::AddCallback( ILogCallback *pCallback )
{
	if ( ! pCallback || ! IsMainThread() )
	{
		return;
	}

	for ( std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it )
	{
		if ( *it == pCallback )
		{
			// the callback is already registered
			return;
		}
	}

	m_callbacks.push_back( pCallback );
}

void EngineLog::Impl::RemoveCallback( ILogCallback *pCallback )
{
	if ( ! pCallback || ! IsMainThread() )
	{
		return;
	}

	for ( std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if ( *it == pCallback )
		{
			it = m_callbacks.erase( it );
		}
		else
		{
			++it;
		}
	}
}

static void LogInitInfo( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	gLauncher->pLog->LogToStdOutV( format, args );
	va_end( args );
}

static void LogInitWarning( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	gLauncher->pLog->LogToStdErrV( format, args, "Warning: " );
	va_end( args );
}

static void LogInitError( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	gLauncher->pLog->LogToStdErrV( format, args, "Error: " );
	va_end( args );
}

static bool CreateLogBackup( HANDLE hFile, const char *path, const char *fileName )
{
	// read first line from the existing log file
	char lineBuffer[1024];
	DWORD lineLength = 0;
	if ( ! ReadFile( hFile, lineBuffer, sizeof lineBuffer, &lineLength, NULL ) )
	{
		LogInitError( "Unable to read from the existing log file: error code %lu", GetLastError() );
		return false;
	}
	else
	{
		size_t i = 0;
		for ( ; i < lineLength; i++ )
		{
			if ( lineBuffer[i] == '\r' || lineBuffer[i] == '\n' )  // CRLF
			{
				break;
			}
		}
		lineBuffer[i] = '\0';
	}

	if ( lineLength == 0 )
	{
		// the existing log file is empty, so no backup is needed
		return true;
	}

	std::string backupName = fileName;
	// remove file extension
	backupName = backupName.substr( 0, backupName.find_last_of( '.' ) );

	const char *prefix = "BackupNameAttachment=";
	const size_t prefixLength = strlen( prefix );  // optimized out

	bool hasNameAttachment = false;

	// parse backup name attachment stored in the existing log file
	if ( strncmp( lineBuffer, prefix, prefixLength ) == 0 )
	{
		const char *nameAttachment = lineBuffer + prefixLength;
		if ( *nameAttachment == '\"' )
		{
			nameAttachment++;

			size_t i = 0;  // length
			while ( nameAttachment[i] != '\0' && nameAttachment[i] != '\"' )
			{
				i++;
			}

			backupName.append( nameAttachment, i );
			hasNameAttachment = true;
		}
	}

	if ( ! hasNameAttachment )
	{
		LogInitWarning( "No backup name attachment found in the existing log file!" );
	}

	std::string backupFolderPath = path;
	backupFolderPath += '\\';
	backupFolderPath += "LogBackups";

	if ( ! CreateDirectoryA( backupFolderPath.c_str(), NULL ) && GetLastError() != ERROR_ALREADY_EXISTS )
	{
		LogInitError( "Unable to create LogBackups folder: error code %lu", GetLastError() );
		return false;
	}

	std::string logFilePath = path;
	logFilePath += '\\';
	logFilePath += fileName;

	std::string backupFilePath = backupFolderPath;
	backupFilePath += '\\';
	backupFilePath += backupName;
	backupFilePath += ".log";

	LogInitInfo( "Creating log file backup..." );

	if ( ! CopyFile( logFilePath.c_str(), backupFilePath.c_str(), FALSE ) )
	{
		LogInitError( "Unable to backup log file: error code %lu", GetLastError() );
		return false;
	}

	return true;
}

bool EngineLog::Impl::OpenLogFile( const char *fileName )
{
	if ( ! gLauncher->rootFolder )
	{
		return false;
	}

	m_logFileName = fileName;

	std::string filePath = gLauncher->rootFolder;
	filePath += '\\';
	filePath += m_logFileName;

	m_hLogFile = CreateFileA( filePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
	                          NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( m_hLogFile == INVALID_HANDLE_VALUE )
	{
		m_hLogFile = NULL;
		LogInitError( "Unable to create log file '%s': error code %lu", filePath.c_str(), GetLastError() );
		return false;
	}
	else if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if ( ! CreateLogBackup( m_hLogFile, gLauncher->rootFolder, m_logFileName.c_str() ) )
		{
			CloseHandle( m_hLogFile );
			return false;
		}

		// clear the existing file
		SetFilePointer( m_hLogFile, 0, NULL, FILE_BEGIN );
		SetEndOfFile( m_hLogFile );
	}

	return true;
}

static bool CheckVerbosity( ILog::ELogType msgType, int verbosity )
{
	switch ( msgType )
	{
		case ILog::eAlways:
		case ILog::eWarningAlways:
		case ILog::eErrorAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			return verbosity >= 0;
		}
		case ILog::eError:
		{
			return verbosity >= 1;
		}
		case ILog::eWarning:
		{
			return verbosity >= 2;
		}
		case ILog::eMessage:
		{
			return verbosity >= 3;
		}
		case ILog::eComment:
		{
			return verbosity >= 4;
		}
	}

	return false;
}

static void AddMsgPrefix( LogBuffer & buffer, ILog::ELogType msgType )
{
	switch ( msgType )
	{
		case ILog::eWarning:
		case ILog::eWarningAlways:
		{
			buffer.append( "$6[Warning] " );
			break;
		}
		case ILog::eError:
		case ILog::eErrorAlways:
		{
			buffer.append( "$4[Error] " );
			break;
		}
		case ILog::eComment:
		{
			buffer.append( "$9" );  // new feature
			break;
		}
		case ILog::eMessage:
		case ILog::eAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			break;
		}
	}
}

void EngineLog::Impl::Log( ILog::ELogType msgType, const char *format, va_list args, int flags )
{
	// yes, Crysis really contains code that calls log with NULL format parameter...
	if ( ! format )
	{
		return;
	}

	int verbosity = GetVerbosity();

	if ( flags & ELogFlags::CONSOLE )
	{
		if ( ! CheckVerbosity( msgType, verbosity ) )
		{
			// don't log to console
			flags &= ~(ELogFlags::CONSOLE);
		}
	}

	if ( flags & ELogFlags::FILE )
	{
		int fileVerbosity = GetFileVerbosity();

		if ( fileVerbosity > verbosity )
		{
			fileVerbosity = verbosity;
		}

		if ( ! CheckVerbosity( msgType, fileVerbosity ) )
		{
			// don't log to file
			flags &= ~(ELogFlags::FILE);
		}
	}

	if ( flags & ELogFlags::FILE || flags & ELogFlags::CONSOLE )
	{
		if ( IsMainThread() )
		{
			LogBuffer buffer;
			AddMsgPrefix( buffer, msgType );
			buffer.append_vf( format, args );

			DoLog( buffer, flags );
		}
		else
		{
			LogTask *pTask = new LogTask( this );
			AddMsgPrefix( pTask->buffer, msgType );
			pTask->buffer.append_vf( format, args );
			pTask->flags = flags;

			gLauncher->pTaskSystem->AddTask( pTask );
		}
	}
}

EngineLog::EngineLog()
: m_impl(new Impl())
{
}

EngineLog::~EngineLog()
{
	delete m_impl;
}

void EngineLog::LogV( const ELogType type, const char *format, va_list args )
{
	m_impl->Log( type, format, args, ELogFlags::FILE | ELogFlags::CONSOLE );
}

void EngineLog::Log( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogV( ILog::eMessage, format, args );
	va_end( args );
}

void EngineLog::LogWarning( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogV( ILog::eWarning, format, args );
	va_end( args );
}

void EngineLog::LogError( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogV( ILog::eError, format, args );
	va_end( args );
}

void EngineLog::Release()
{
}

bool EngineLog::SetFileName( const char *fileName )
{
	return m_impl->OpenLogFile( fileName );
}

const char *EngineLog::GetFileName()
{
	return m_impl->GetLogFileName().c_str();
}

void EngineLog::LogPlus( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	m_impl->Log( ILog::eMessage, format, args, ELogFlags::FILE | ELogFlags::CONSOLE | ELogFlags::APPEND );
	va_end( args );
}

void EngineLog::LogToFile( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	m_impl->Log( ILog::eMessage, format, args, ELogFlags::FILE );
	va_end( args );
}

void EngineLog::LogToFilePlus( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	m_impl->Log( ILog::eMessage, format, args, ELogFlags::FILE | ELogFlags::APPEND );
	va_end( args );
}

void EngineLog::LogToConsole( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	m_impl->Log( ILog::eMessage, format, args, ELogFlags::CONSOLE );
	va_end( args );
}

void EngineLog::LogToConsolePlus( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	m_impl->Log( ILog::eMessage, format, args, ELogFlags::CONSOLE | ELogFlags::APPEND );
	va_end( args );
}

void EngineLog::UpdateLoadingScreen( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	LogV( ILog::eMessage, format, args );
	va_end( args );

	ISystem *pSystem = gLauncher->pSystem;

	if ( ! pSystem || ! IsMainThread() )
	{
		return;
	}

	//IRenderer *pRenderer = pSystem->GetIRenderer();

	if ( ! pSystem->IsEditor() )
	{
		ISystem::ILoadingProgressListener *pLoadingProgressListener = pSystem->GetLoadingProgressListener();
		if ( pLoadingProgressListener )
		{
			pLoadingProgressListener->OnLoadingProgress( 0 );
		}

		//if ( pRenderer )  // original code doesn't check if renderer exists
		//{
		//	// really no idea why this thing is here
		//	pRenderer->EF_Query( EFQ_RecurseLevel, 0 );
		//}
	}

	// the following code is completely useless on server, so it's disabled here
	// note that even null renderer returns valid window handle because of that hidden OpenGL debug renderer thing

	// original code uses result of IRenderer::Init stored in CSystem
	//HWND hWnd = (pRenderer) ? static_cast<HWND>( pRenderer->GetHWND() ) : NULL;

	//if ( hWnd && IsWindow( hWnd ) )
	//{
	//	MSG msg;
	//	while ( PeekMessageA( &msg, hWnd, 0, 0, PM_REMOVE ) )
	//	{
	//		TranslateMessage( &msg );
	//		DispatchMessage( &msg );
	//	}
	//}
}

void EngineLog::RegisterConsoleVariables()
{
	m_impl->RegisterConsoleVariables();
}

void EngineLog::UnregisterConsoleVariables()
{
	m_impl->UnregisterConsoleVariables();
}

void EngineLog::SetVerbosity( int verbosity )
{
	m_impl->SetVerbosity( verbosity );
}

int EngineLog::GetVerbosityLevel()
{
	return m_impl->GetVerbosity();
}

void EngineLog::AddCallback( ILogCallback *pCallback )
{
	m_impl->AddCallback( pCallback );
}

void EngineLog::RemoveCallback( ILogCallback *pCallback )
{
	m_impl->RemoveCallback( pCallback );
}

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
: m_pEngineLog()
{
}

Log::~Log()
{
	if ( m_pEngineLog )
	{
		delete m_pEngineLog;
	}
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

bool Log::InitEngineLog()
{
	std::string logFileName = CmdLine::GetArgValue( "-logfile", LOG_DEFAULT_FILE_NAME );
	gLauncher->defaultLogVerbosity = CmdLine::GetArgValueInt( "-verbosity", LOG_DEFAULT_VERBOSITY );

	if ( ! m_pEngineLog )
	{
		m_pEngineLog = new EngineLog();
	}

	return m_pEngineLog->SetFileName( logFileName.c_str() );
}

