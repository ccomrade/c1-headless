# c1-headless

C1-Headless is a Crysis dedicated server launcher without the ineffective server console window. It's based on
[c1-launcher](https://github.com/ccomrade/c1-launcher/) project and it accepts the same command line arguments as the original
dedicated server launcher. It can be used as a drop-in replacement of the original dedicated server launcher. The goal is to
provide a better Crysis server launcher optimized for running under Wine on Linux servers. It's also fully compatible with
Windows. The old compiler used by Crytek to build Crysis (VS2005) is still supported here.

![Screenshot](Screenshot.png)

### Launcher API
This launcher has special API that provides external access to its additional features. The API is defined in `ILauncher`
interface and any Crysis mod can use it. Future launcher versions may add new features to the API.

To use it in your mod, take the following headers from `Code/Launcher`:
```
ILauncher.h
ILauncherTask.h
```

And then use the following code to access the API:
```c++
ILauncher *pLauncher = NULL;

HMODULE hExe = GetModuleHandleA( NULL );
ILauncher::TGetFunc pGetILauncher = (ILauncher::TGetFunc) GetProcAddress( hExe, "GetILauncher" );
if ( pGetILauncher )
{
	pLauncher = pGetILauncher();
}

if ( pLauncher )
{
	// Launcher API is available
}
else
{
	// Launcher API is not available
}
```

### Required DLLs
Here is a complete list of DLL files required to run Crysis server using this launcher. Note that DLL files always provided by
Windows operating system are not listed here.

32-bit version:
```
Bin32/
├── Cry3DEngine.dll
├── CryAction.dll
├── CryAISystem.dll
├── CryAnimation.dll
├── CryEntitySystem.dll
├── CryFont.dll
├── CryGame.dll
├── CryNetwork.dll
├── CryPhysics.dll
├── CryRenderNULL.dll
├── CryScriptSystem.dll
├── CrySystem.dll
├── ijl15.dll
├── IntelLaptopGaming.dll
└── msvcr80.dll
```

64-bit version:
```
Bin64/
├── Cry3DEngine.dll
├── CryAction.dll
├── CryAISystem.dll
├── CryAnimation.dll
├── CryEntitySystem.dll
├── CryFont.dll
├── CryGame.dll
├── CryNetwork.dll
├── CryPhysics.dll
├── CryRenderNULL.dll
├── CryScriptSystem.dll
├── CrySystem.dll
└── msvcr80.dll
```
