/**
 * @file
 * @brief Headless dedicated server launcher.
 */

#include <stdio.h>
#include <stdarg.h>

// CryEngine headers
#include "CryModuleDefs.h"
#include "platform_impl.h"
#include "platform.h"
#include "IGameStartup.h"
#include "ISystem.h"

// Launcher headers
#include "LauncherEnv.h"
#include "Patch.h"
#include "CPU.h"
#include "Util.h"

#include "config.h"

#ifdef BUILD_64BIT
#define LAUNCHER_BUILD_VERSION C1HEADLESS_VERSION_STRING " 64-bit"
#else
#define LAUNCHER_BUILD_VERSION C1HEADLESS_VERSION_STRING " 32-bit"
#endif

class GlobalLauncherEnv
{
	LauncherEnv m_env;

public:
	GlobalLauncherEnv()
	{
		memset( &m_env, 0, sizeof m_env );
		gLauncher = &m_env;

		gLauncher->mainThreadID = GetCurrentThreadId();
	}

	~GlobalLauncherEnv()
	{
		gLauncher = NULL;
	}
};

static void LogInfo( const char *msg )
{
	puts( msg );

	fflush( stdout );
}

static void LogError( const char *msg )
{
	printf( "Error: %s\n", msg );

	fflush( stdout );
}

static void Log( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	vprintf( format, args );
	va_end( args );

	putchar( '\n' );

	fflush( stdout );
}

struct CUserCallback : public ISystemUserCallback
{
	bool OnError( const char *szErrorString ) override
	{
		LogError( szErrorString );
		return true;  // quit
	}

	void OnSaveDocument() override
	{
	}

	void OnProcessSwitch() override
	{
	}

	void OnInitProgress( const char *sProgressMsg ) override
	{
		LogInfo( sProgressMsg );
	}

	void OnInit( ISystem *pSystem ) override
	{
	}

	void OnShutdown() override
	{
		LogInfo( "Quit" );
	}

	void OnUpdate() override
	{
	}

	void GetMemoryUsage( ICrySizer *pSizer ) override
	{
	}
};

class DLLHandleGuard
{
	HMODULE m_handle;

public:
	DLLHandleGuard( HMODULE handle )
	: m_handle(handle)
	{
	}

	~DLLHandleGuard()
	{
		if ( m_handle )
		{
			FreeLibrary( m_handle );
		}
	}

	operator bool() const
	{
		return m_handle != NULL;
	}

	operator HMODULE()
	{
		return m_handle;
	}
};

static int RunServer( HMODULE libCryGame )
{
	IGameStartup::TEntryFunction fCreateGameStartup;

	fCreateGameStartup = (IGameStartup::TEntryFunction) GetProcAddress( libCryGame, "CreateGameStartup" );
	if ( fCreateGameStartup == NULL )
	{
		LogError( "The CryGame DLL is not valid!" );
		return -1;
	}

	IGameStartup *pGameStartup = fCreateGameStartup();
	if ( pGameStartup == NULL )
	{
		LogError( "Unable to create the GameStartup interface!" );
		return -1;
	}

	CUserCallback userCallback;

	const char *cmdLine = GetCommandLineA();
	const size_t cmdLineLength = strlen( cmdLine );

	SSystemInitParams params;
	memset( &params, 0, sizeof params );

	params.hInstance = GetModuleHandle( NULL );
	params.pUserCallback = &userCallback;  // disables dedicated server console window
	params.sLogFileName = "Server.log";
	params.bDedicatedServer = true;  // better than adding "-dedicated" to the command line

	if ( cmdLineLength < sizeof params.szSystemCmdLine )
	{
		strcpy( params.szSystemCmdLine, cmdLine );
	}
	else
	{
		LogError( "Command line is too long!" );
		return -1;
	}

	// init engine
	IGameRef gameRef = pGameStartup->Init( params );
	if ( gameRef == NULL )
	{
		LogError( "Engine initialization failed!" );
		pGameStartup->Shutdown();
		return -1;
	}

	// init CryEngine global environment for the launcher
	ModuleInitISystem( params.pSystem );

	LogInfo( "Server started" );

	// enter update loop
	int status = pGameStartup->Run( NULL );
	LogInfo( "Engine exit code: %d", status );

	pGameStartup->Shutdown();

	return (status != 0) ? -1 : 0;
}

static int InstallMemoryPatches( int version, void *libCryAction, void *libCryNetwork, void *libCrySystem )
{
	// CryAction

	if ( PatchGameplayStats( libCryAction, version ) < 0 )
		return -1;

	// CryNetwork

	if ( PatchDuplicateCDKey( libCryNetwork, version ) < 0 )
		return -1;

	// CrySystem

	if ( PatchUnhandledExceptions( libCrySystem, version ) < 0 )
		return -1;

	if ( CPU::IsAMD() && ! CPU::Has3DNow() )
	{
		// Dedicated server usually doesn't execute any code with 3DNow! instructions, but we should still make sure that
		// ISystem::GetCPUFlags always returns correct flags.

		if ( PatchDisable3DNow( libCrySystem, version ) < 0 )
			return -1;
	}

	return 0;
}

int main()
{
	GlobalLauncherEnv env;

	LogInfo( "C1-Headless " LAUNCHER_BUILD_VERSION );

	DLLHandleGuard libCryGame = LoadLibraryA( "CryGame.dll" );
	if ( ! libCryGame )
	{
		LogError( "Unable to load the CryGame DLL!" );
		return 1;
	}

	DLLHandleGuard libCryAction = LoadLibraryA( "CryAction.dll" );
	if ( ! libCryAction )
	{
		LogError( "Unable to load the CryAction DLL!" );
		return 1;
	}

	DLLHandleGuard libCryNetwork = LoadLibraryA( "CryNetwork.dll" );
	if ( ! libCryNetwork )
	{
		LogError( "Unable to load the CryNetwork DLL!" );
		return 1;
	}

	// no heap allocations should be done before CrySystem is loaded
	DLLHandleGuard libCrySystem = LoadLibraryA( "CrySystem.dll" );
	if ( ! libCrySystem )
	{
		LogError( "Unable to load the CrySystem DLL!" );
		return 1;
	}

	// obtain game build number from CrySystem DLL
	int gameVersion = Util::GetCrysisGameVersion( libCrySystem );
	if ( gameVersion < 0 )
	{
		LogError( "Unable to obtain game version from the CrySystem DLL!" );
		return 1;
	}
	else
	{
		gLauncher->gameVersion = gameVersion;
		Log( "Detected game version: %d", gameVersion );
	}

	// check version of the game and apply memory patches
	switch ( gameVersion )
	{
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		{
			if ( InstallMemoryPatches( gameVersion, libCryAction, libCryNetwork, libCrySystem ) < 0 )
			{
				LogError( "Unable to apply memory patch!" );
				return 1;
			}
			else
			{
				LogInfo( "All memory patches installed successfully" );
			}
			break;
		}
		default:
		{
			LogError( "Unsupported version of the game!" );
			return 1;
		}
	}

	// launch the server
	int status = RunServer( libCryGame );

	return (status < 0) ? 1 : 0;
}

