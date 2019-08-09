/**
 * @file
 * @brief Implementation of game engine listener.
 */

// Launcher headers
#include "EngineListener.h"
#include "LauncherEnv.h"
#include "TaskSystem.h"
#include "Log.h"

bool EngineListener::OnError( const char *szErrorString )
{
	gLauncher->pLog->LogToStdErr( "Error: %s", szErrorString );
	// quit
	return true;
}

void EngineListener::OnSaveDocument()
{
}

void EngineListener::OnProcessSwitch()
{
}

void EngineListener::OnInitProgress( const char *sProgressMsg )
{
	gLauncher->pLog->LogToStdOut( "%s", sProgressMsg );
}

void EngineListener::OnInit( ISystem *pSystem )
{
	gLauncher->pSystem = pSystem;
}

void EngineListener::OnShutdown()
{
	gLauncher->pLog->LogToStdOut( "Quit" );
}

void EngineListener::OnUpdate()
{
	if ( gLauncher->pTaskSystem )
	{
		gLauncher->pTaskSystem->ExecuteWaitingTasks();
	}
}

void EngineListener::GetMemoryUsage( ICrySizer *pSizer )
{
}

