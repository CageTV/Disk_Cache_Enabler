// C Standard
#include <cstdio>
#include <cstdlib>

// SKSE
#include <skse/common/IPrefix.h>
#include <skse/skse64/PluginAPI.h>
#include <skse/skse64_common/skse_version.h>

// Detours
#include <detours/detours.h>

#define SKSEAPI extern "C" __declspec(dllexport)

#define PLUGINNAME "Skyrim Disk Cache Enabler"
// SKSE's MAKE_EXE_VERSION layout: major 8 bits, minor 8 bits, build 12 bits
#define PLUGINVERSION ((1 & 0xFF) << 24 | (2 & 0xFF) << 16 | (0 & 0xFFF) << 4)

// This plugin never touches game memory, it only wraps kernel32's CreateFileA, so it does not depend on
// any game version. kVersionIndependent_Signatures tells SKSE to skip the compatibleVersions check, which
// is what lets it load on every runtime from 1.5.97 up to 1.7.x.
SKSEAPI const SKSEPluginVersionData SKSEPlugin_Version = {
    SKSEPluginVersionData::kVersion,
    PLUGINVERSION,
    PLUGINNAME,
    "Archost",
    "",
    SKSEPluginVersionData::kVersionIndependentEx_NoStructUse,
    SKSEPluginVersionData::kVersionIndependent_Signatures,
    {0},
    0};

static HANDLE(WINAPI *CreateFileA_orig)(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttribtes,
    HANDLE                hTemplateFile) = CreateFileA;

static HANDLE WINAPI CreateFileA_hook(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttribtes,
    HANDLE                hTemplateFile)
{
  // Let the OS file cache hold what the game reads (archives are read in small, random chunks)
  dwFlagsAndAttribtes &= ~(FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN);
  dwFlagsAndAttribtes |= FILE_FLAG_RANDOM_ACCESS;

  return CreateFileA_orig(
      lpFilename,
      dwDesiredAccess,
      dwShareMode,
      lpSecurityAttributes,
      dwCreationDisposition,
      dwFlagsAndAttribtes,
      hTemplateFile);
}

SKSEAPI bool SKSEPlugin_Query(const SKSEInterface *skse, PluginInfo *info)
{
  info->infoVersion = PluginInfo::kInfoVersion;
  info->name        = PLUGINNAME;
  info->version     = PLUGINVERSION;

  return true;
}

// Installed from the plugin load callback, not DllMain: Detours suspends and resumes threads while it
// patches, which is not something to do under the loader lock.
SKSEAPI bool SKSEPlugin_Load(const SKSEInterface *skse)
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(&(PVOID &)CreateFileA_orig, CreateFileA_hook);
  LONG error = DetourTransactionCommit();

  if ( error != NO_ERROR )
  {
    OutputDebugStringA("Disk Cache Enabler: CreateFileA() hook failed\n");
    MessageBoxA(NULL, "Failed to attach!", "Disk Cache Enabler", MB_OK);
    return false;
  }

  return true;
}

// DllMain only has to be a harmless stub: no work on attach, nothing to undo on detach (the process is
// exiting, and detouring back under the loader lock can deadlock)
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID reserved)
{
  if ( dwReason == DLL_PROCESS_ATTACH )
  {
    DisableThreadLibraryCalls(hInstance);
  }
  return TRUE;
}
