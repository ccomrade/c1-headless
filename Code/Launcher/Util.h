/**
 * @file
 * @brief Utilities.
 */

#pragma once

#include <stddef.h>  // size_t

int FillNOP( void *address, size_t length );
int FillMem( void *address, const void *data, size_t length );

int GetCrysisGameVersion( void *lib );

bool HasAMDProcessor();
bool Is3DNowSupported();

