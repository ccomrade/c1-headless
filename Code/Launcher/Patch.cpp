/**
 * @file
 * @brief Implementation of functions for patching Crysis code.
 */

// Launcher headers
#include "Patch.h"
#include "Util.h"
#include "NULLRenderAuxGeom.h"

using namespace Util;

static CNULLRenderAuxGeom g_renderAuxGeom;

/**
 * @brief Disables automatic creation of "gameplaystatsXXX.txt" files.
 * The "dump_stats" console command can still be used to create these files manually.
 * @param libCryAction CryAction DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchGameplayStats( void *libCryAction, int gameVersion )
{
	const unsigned char code[] = {
		0xC3,  // ret
		0x90,  // nop
		0x90,  // nop
		0x90,  // nop
		0x90   // nop
	};

	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillMem( CalculateAddress( libCryAction, 0x2F21D6 ), code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( CalculateAddress( libCryAction, 0x2F59E6 ), code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( CalculateAddress( libCryAction, 0x2FA686 ), code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( CalculateAddress( libCryAction, 0x2FA976 ), code, sizeof code ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCryAction, 0x2016ED ), 0x7 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCryAction, 0x203EBD ), 0x7 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryAction, 0x20668D ), 0x7 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryAction, 0x20605D ), 0x7 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Prevents server from kicking players with the same CD key.
 * This is server-side patch.
 * @param libCryNetwork CryNetwork DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDuplicateCDKey( void *libCryNetwork, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0xE4858 ), 0x47 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0xE5628 ), 0x47 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0xE0188 ), 0x47 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0xE0328 ), 0x47 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x608CE ), 0x4 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x5DE79 ), 0x4 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x60EF2 ), 0x4 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x606A5 ), 0x4 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables server profiler - "server_profile.txt" file.
 * Only 32-bit version has server profiler.
 * @param libCryNetwork CryNetwork DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchServerProfiler( void *libCryNetwork, int gameVersion )
{
#ifndef BUILD_64BIT
	switch ( gameVersion )
	{
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x9F435 ), 0x5 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x9CA81 ), 0x5 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x9C665 ), 0x5 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryNetwork, 0x9BE2E ), 0x5 ) < 0 )
				return -1;
			break;
		}
		default:
		{
			return -1;
		}
	}
#endif

	return 0;
}

/**
 * @brief Disables use of 3DNow! instructions.
 * This patch fixes the well known crash of 32-bit Crysis on modern AMD processors.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDisable3DNow( void *libCrySystem, int gameVersion )
{
	const unsigned char flags = 0x18;  // default processor feature flags without CPUF_3DNOW flag

	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0xA1AF ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0xA0FF ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0xA0BF ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0xA0FF ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0x9432 ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0x9472 ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0x9412 ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( CalculateAddress( libCrySystem, 0x93E2 ), &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables custom handling of unhandled exceptions.
 * This patch prevents the engine from enabling its own unhandled exception callback via SetUnhandledExceptionFilter function.
 * The callback is used to do various things after crash, such as calling ReportFault function or creating minidump. It depends
 * on value of sys_WER cvar (0, 1, 2). However, everything done in this callback is completely useless and causes only problems.
 * Even MSDN documentation recommends that applications shouldn't try do such things at their own. Instead, they should let
 * operating system handle fatal errors for them.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchUnhandledExceptions( void *libCrySystem, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x22986 ), 0x6  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x22992 ), 0x7  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x45C8A ), 0x16 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x232C6 ), 0x6  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x232D2 ), 0x7  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x4725A ), 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x22966 ), 0x6  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x22972 ), 0x7  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x466E9 ), 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x22946 ), 0x6  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x22952 ), 0x7  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x467A5 ), 0x16 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x182B7 ), 0x5  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x182C2 ), 0xC  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x59F58 ), 0x13 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x18437 ), 0x5  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x18442 ), 0xC  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x5A4D8 ), 0x13 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x18217 ), 0x5  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x18222 ), 0xC  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x5A2B8 ), 0x13 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCrySystem, 0x17D67 ), 0x5  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x17D72 ), 0xC  ) < 0
			  || FillNOP( CalculateAddress( libCrySystem, 0x59DF8 ), 0x13 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables existing RenderAuxGeom in CryRenderNULL and replaces it with custom empty implementation.
 * This patch disables all that useless debug renderer junk in CryRenderNULL.
 * @param libCryRenderNULL CryRenderNULL DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchRenderAuxGeom( void *libCryRenderNULL, int gameVersion )
{
	const unsigned char code[] = {
		0xC3,  // ret
	#ifdef BUILD_64BIT
		0x90,  // nop
	#endif
		0x90,  // nop
		0x90,  // nop
		0x90,  // nop
		0x90,  // nop
		0x90   // nop
	};

	void *pAuxGeom = &g_renderAuxGeom;

	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x21A5 ), 0x32 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16BE ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16D0 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xBBD58 ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x21B5 ), 0x32 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16CE ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16E0 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xBBD58 ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x21C5 ), 0x32 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16BE ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16D0 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xBBD98 ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x2205 ), 0x32 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16CE ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x16E0 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xBBD98 ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		case 5879:  // same offsets
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x208C ), 0x18 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x1895 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x18A9 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xD2D0C ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x208C ), 0x18 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x1895 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x18A9 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xD2D2C ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( CalculateAddress( libCryRenderNULL, 0x208C ), 0x18 ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x1895 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0x18A9 ), code, sizeof code ) < 0
			  || FillMem( CalculateAddress( libCryRenderNULL, 0xD3D2C ), &pAuxGeom, sizeof pAuxGeom ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

