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

#define PLUGINNAME    "Skyrim Disk Cache Enabler"
#define PLUGINVERSION 1 << 8 | 1

SKSEAPI const SKSEPluginVersionData SKSEPlugin_Version = {
    SKSEPluginVersionData::kVersion,
    PLUGINVERSION,
    PLUGINNAME,
    "Archost",
    "",
    SKSEPluginVersionData::kVersionIndependentEx_NoStructUse,
    SKSEPluginVersionData::kVersionIndependent_Signatures,
    {RUNTIME_VERSION_1_6_659_GOG,
     RUNTIME_VERSION_1_6_640,
     RUNTIME_VERSION_1_5_97,
     0},
    0};

static HANDLE(WINAPI *CreateFileA_orig)(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttribtes,
    HANDLE                hTemplateFile) = CreateFileA;

HANDLE CreateFileA_hook(
    LPCSTR                lpFilename,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttribtes,
    HANDLE                hTemplateFile)
{
  dwFlagsAndAttribtes &= ~(FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN);
  dwFlagsAndAttribtes |= FILE_FLAG_RANDOM_ACCESS;

  printf("CreateFileA: %s\n", lpFilename);

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

SKSEAPI bool SKSEPlugin_Load(const SKSEInterface *skse)
{
  // FILE *stdof;
  // AllocConsole();
  // freopen_s(&stdof, "CONOUT$", "w", stdout);
  return true;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID TESVIWhen)
{
  LONG error;

  if ( DetourIsHelperProcess() )
  {
    return TRUE;
  }

  switch ( dwReason )
  {
  case DLL_PROCESS_ATTACH:
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID &)CreateFileA_orig, CreateFileA_hook);
    error = DetourTransactionCommit();

    if ( error == NO_ERROR )
    {
      puts("CreateFileA() Hooked");
    }
    else
    {
      MessageBoxA(NULL, "Failed to attach!", "Disk Cache Enabler", MB_OK);
      puts("CreateFileA() Hook failed");
    }
    break;
  case DLL_PROCESS_DETACH:
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)CreateFileA_orig, CreateFileA_hook);
    error = DetourTransactionCommit();

    break;
  }
  return TRUE;
}
