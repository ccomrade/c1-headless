/**
 * @file
 * @brief Implementation of utilities.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Launcher headers
#include "Util.h"

/**
 * @brief Fills read-only memory region with x86 NOP instruction.
 * @param address Memory region address.
 * @param length Memory region size in bytes.
 * @return 0 if no error occurred, otherwise -1.
 */
int Util::FillNOP( void *address, size_t length )
{
	DWORD oldProtection;

	if ( VirtualProtect( address, length, PAGE_EXECUTE_READWRITE, &oldProtection ) == 0 )
		return -1;

	// 0x90 is opcode of NOP instruction for both x86 and x86_64
	memset( address, '\x90', length );

	if ( VirtualProtect( address, length, oldProtection, &oldProtection ) == 0 )
		return -1;

	return 0;
}

/**
 * @brief Fills read-only memory region with custom data.
 * @param address Memory region address.
 * @param data The data copied to the memory region.
 * @param length Size of the data in bytes.
 * @return 0 if no error occurred, otherwise -1.
 */
int Util::FillMem( void *address, const void *data, size_t length )
{
	DWORD oldProtection;

	if ( VirtualProtect( address, length, PAGE_EXECUTE_READWRITE, &oldProtection ) == 0 )
		return -1;

	memcpy( address, data, length );

	if ( VirtualProtect( address, length, oldProtection, &oldProtection ) == 0 )
		return -1;

	return 0;
}

/**
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param lib Handle to any loaded Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int Util::GetCrysisGameVersion( void *lib )
{
	// VERSIONINFO resource always has ID 1
	HRSRC versionResInfo = FindResource( static_cast<HMODULE>( lib ), MAKEINTRESOURCE( 1 ), RT_VERSION );
	if ( versionResInfo == NULL )
		return -1;

	HGLOBAL versionResData = LoadResource( static_cast<HMODULE>( lib ), versionResInfo );
	if ( versionResData == NULL )
		return -1;

	void *versionRes = LockResource( versionResData );  // this function does nothing
	if ( versionRes == NULL )
		return -1;

	if ( memcmp( CalculateAddress( versionRes, 0x6 ), L"VS_VERSION_INFO", 0x20 ) != 0 )
		return -1;

	VS_FIXEDFILEINFO *pFileInfo = static_cast<VS_FIXEDFILEINFO*>( CalculateAddress( versionRes, 0x6 + 0x20 + 0x2 ) );
	if ( pFileInfo->dwSignature != 0xFEEF04BD )
		return -1;

	return LOWORD( pFileInfo->dwFileVersionLS );
}

