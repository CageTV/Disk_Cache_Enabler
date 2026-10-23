# Disk Cache Enabler

SKSE plugin for Skyrim Special Edition / Anniversary Edition. It wraps `CreateFileA` and removes `FILE_FLAG_NO_BUFFERING` and `FILE_FLAG_SEQUENTIAL_SCAN` from every file the game opens, adding `FILE_FLAG_RANDOM_ACCESS`. That lets Windows keep the game's archives in its file cache, which cuts disk reads and hitching.

Original mod by Archost ([Nexus 100975](https://www.nexusmods.com/skyrimspecialedition/mods/100975)), ISC licensed.

## Installing

Install the zip with your mod manager. The DLL goes to `Data/SKSE/Plugins/diskCacheEnabler.dll`. It needs SKSE64 and nothing else, and it does not depend on a game version (1.5.97 through 1.7.x).

## Building

Needs Visual Studio (x64 tools) and CMake. Run `build.cmd`; the DLL ends up in `build/src/diskCacheEnabler.dll`. Detours and the SKSE headers are vendored in `deps/`.
