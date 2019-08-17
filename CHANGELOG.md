# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.1] - 2019-08-17
### Added
- Launcher API that can be used by SSM. See README for more information.
- Custom implementation of CryEngine validator that does nothing. The original validator shows blocking message box.
- Custom implementation of CryEngine log:
    - Highly effective and fully compatible with the original implementation, but without any of its bugs.
    - Created from scratch with only certain parts reverse engineered from the original implementation to maintain compatibility.
    - Log messages from any thread other than main are now logged properly instead of being silently dropped.
    - Crysis color codes and control characters are automatically discarded when writing to the log file.
    - Uses custom [printf library](https://github.com/mpaland/printf) instead of
      [broken](https://stackoverflow.com/questions/18107426/printf-format-for-unsigned-int64-on-windows) printf functions
      provided by MSVC.
    - `eComment` log messages have `$9` (gray) color prefix.
    - Verbosity cvars support value `-1` (drop all messages including `eAlways`). This is feature from CryEngine V.
    - Default verbosity level can be set using the new `-verbosity` command line parameter.
    - Log file name can still be changed using the existing `-logfile` command line parameter.
    - Single log file cannot be used concurrently by multiple Crysis instances anymore to prevent log corruption.
- Patch that disables useless `server_profile.txt` file.
- Patch that disables useless debug renderer in CryRenderNULL:
    - No OpenGL code is now executed on the server. However, CryRenderNULL still depends on OpenGL DLLs.
    - No VRAM is finally used by the server.
    - The hidden debug renderer window is not being created anymore.
    - Removes `r_debug_renderer_show_window` and `r_debug_renderer_set_eye_pos` console commands.
    - Slightly reduces CPU and memory usage of the server.

## [1.0] - 2019-07-14
