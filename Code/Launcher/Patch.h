/**
 * @file
 * @brief Functions for patching Crysis code.
 */

#pragma once

// CryAction
int PatchGameplayStats( void *libCryAction, int gameVersion );

// CryNetwork
int PatchDuplicateCDKey( void *libCryNetwork, int gameVersion );
int PatchServerProfiler( void *libCryNetwork, int gameVersion );

// CrySystem
int PatchDisable3DNow( void *libCrySystem, int gameVersion );
int PatchUnhandledExceptions( void *libCrySystem, int gameVersion );

