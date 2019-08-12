/**
 * @file
 * @brief Implementation of command line.
 */

#include <ctype.h>
#include <stddef.h>  // size_t
#include <stdlib.h>  // atoi

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Launcher headers
#include "CmdLine.h"
#include "LauncherEnv.h"
#include "Log.h"

static const char *GetCmdLineWithoutAppName()
{
	const char *cmdLine = GetCommandLineA();

	// skip program name
	if ( *cmdLine == '\"' )
	{
		for ( cmdLine++; *cmdLine && *cmdLine != '\"'; cmdLine++ );
	}
	else if ( *cmdLine == '\'' )
	{
		for ( cmdLine++; *cmdLine && *cmdLine != '\''; cmdLine++ );
	}
	else
	{
		for ( cmdLine++; *cmdLine && ! isspace( *cmdLine ); cmdLine++ );
	}

	if ( *cmdLine )
	{
		cmdLine++;

		while ( isspace( *cmdLine ) )
		{
			cmdLine++;
		}
	}

	return cmdLine;
}

static const char *GetArgValueBegin( const char *arg )
{
	const char *cmdLine = GetCmdLineWithoutAppName();

	// find the argument
	for ( ; *cmdLine; cmdLine++ )
	{
		if ( tolower( *cmdLine ) == tolower( *arg ) )
		{
			const char *a = arg + 1;
			for ( cmdLine++; *cmdLine && *a; cmdLine++, a++ )
			{
				if ( tolower( *cmdLine ) != tolower( *a ) )
				{
					break;
				}
			}

			if ( ! *a )
			{
				// reached end of argument name
				if ( ! *cmdLine || isspace( *cmdLine ) )
				{
					return cmdLine;
				}
			}
		}
	}

	return NULL;
}

bool CmdLine::HasArg( const char *arg )
{
	return GetArgValueBegin( arg ) != NULL;
}

std::string CmdLine::GetArgValue( const char *arg, const char *defaultValue )
{
	const char *valueBegin = GetArgValueBegin( arg );
	if ( valueBegin )
	{
		while ( isspace( *valueBegin ) )
		{
			valueBegin++;
		}

		if ( *valueBegin )
		{
			size_t i = 0;

			if ( *valueBegin == '\"' )
			{
				valueBegin++;

				while ( valueBegin[i] && valueBegin[i] != '\"' )
				{
					i++;
				}
			}
			else if ( *valueBegin == '\'' )
			{
				valueBegin++;

				while ( valueBegin[i] && valueBegin[i] != '\'' )
				{
					i++;
				}
			}
			else
			{
				while ( valueBegin[i] && ! isspace( valueBegin[i] ) )
				{
					i++;
				}
			}

			return std::string( valueBegin, i );
		}
	}

	return (defaultValue) ? std::string( defaultValue ) : std::string();
}

int CmdLine::GetArgValueInt( const char *arg, int defaultValue )
{
	std::string value = GetArgValue( arg );
	return (value.empty()) ? defaultValue : atoi( value.c_str() );
}

void CmdLine::Log()
{
	gLauncher->pLog->LogToStdOut( "Command line: [%s]", GetCmdLineWithoutAppName() );
}

