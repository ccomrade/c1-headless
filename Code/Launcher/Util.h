/**
 * @file
 * @brief Utilities.
 */

#pragma once

#include <stddef.h>

namespace Util
{
	char *FindStringNoCase( char *buffer, const char *string );

	int FillNOP( void *address, size_t length );
	int FillMem( void *address, const void *data, size_t length );

	int GetCrysisGameVersion( void *lib );

	inline void *CalculateAddress( void *base, size_t offset )
	{
		return static_cast<unsigned char*>( base ) + offset;
	}

	inline const void *CalculateAddress( const void *base, size_t offset )
	{
		return static_cast<const unsigned char*>( base ) + offset;
	}
}

