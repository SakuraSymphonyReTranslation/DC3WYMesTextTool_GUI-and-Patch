/*
 * DC3WY Patch Launcher (Built-in Locale Emulator)
 *
 * For DC3WY: Uses LEProc.exe (bundled) to start the game under Japanese locale,
 * then injects DC3WYPatch.dll into the running game process.
 *
 * If LEProc.exe / LocaleEmulator.dll are not present (or running under Wine),
 * falls back to plain CreateProcess + registry locale trick.
 *
 * Save directory: AdvData/Savegame is created automatically on launch.
 */

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <string>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <tlhelp32.h>

typedef const char *(CDECL *pfnWineGetVersion)(void);

// ---------------------------------------------------------------------------
// Wine detection
// ---------------------------------------------------------------------------
static bool IsRunningUnderWine() {
  HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
  if (!hNtdll) return false;
  pfnWineGetVersion pWineGetVersion =
      (pfnWineGetVersion)GetProcAddress(hNtdll, "wine_get_version");
  return (pWineGetVersion != NULL);
}

// ---------------------------------------------------------------------------
// Resolve the directory of this executable
// ---------------------------------------------------------------------------
static bool BuildModuleDirectory(char *outDir, size_t outDirSize) {
  char modulePath[MAX_PATH] = {0};
  DWORD len = GetModuleFileNameA(NULL, modulePath, MAX_PATH);
  if (len == 0 || len >= MAX_PATH) return false;
  char *sep = strrchr(modulePath, '\\');
  if (!sep) return false;
  *sep = '\0';
  return (strcpy_s(outDir, outDirSize, modulePath) == 0);
}

// ---------------------------------------------------------------------------
// Wide string helpers
// ---------------------------------------------------------------------------
static std::wstring AnsiToWide(const char *text) {
  if (!text || !*text) return std::wstring();
  int wlen = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);
  if (wlen <= 0) return std::wstring();
  std::wstring out((size_t)(wlen - 1), L'\0');
  MultiByteToWideChar(CP_ACP, 0, text, -1, out.data(), wlen);
  return out;
}

// ---------------------------------------------------------------------------
// Ensure a directory tree exists (recursive mkdir)
// ---------------------------------------------------------------------------
static void EnsureDirectoryTreeW(const wchar_t *path) {
  if (!path || !*path) return;
  wchar_t tmp[MAX_PATH] = {};
  wcscpy_s(tmp, path);
  for (wchar_t *p = tmp; *p; p++) {
    if (*p != L'\\' && *p != L'/') continue;
    if (p == tmp) continue;
    if (p == tmp + 2 && tmp[1] == L':') continue;
    wchar_t saved = *p;
    *p = L'\0';
    CreateDirectoryW(tmp, NULL);
    *p = saved;
  }
  CreateDirectoryW(tmp, NULL);
}

// ---------------------------------------------------------------------------
// DC3WY: ensure AdvData/Savegame and system saved-games folders exist
// ---------------------------------------------------------------------------
static void EnsureSaveLayoutForDC3WY(const char *currentDir) {
  std::wstring gameDir = AnsiToWide(currentDir);
  if (!gameDir.empty()) {
    EnsureDirectoryTreeW((gameDir + L"\\AdvData\\Savegame").c_str());
  }

  PWSTR savedGames = NULL;
  if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT,
                                     NULL, &savedGames)) &&
      savedGames) {
    EnsureDirectoryTreeW(
        (std::wstring(savedGames) + L"\\サーカス\\D．C．Ⅲ W．Y．").c_str());
    EnsureDirectoryTreeW(
        (std::wstring(savedGames) + L"\\サーカス\\D.C.III W.Y.").c_str());
    EnsureDirectoryTreeW(
        (std::wstring(savedGames) + L"\\Circus\\D.C.III W.Y.").c_str());
    CoTaskMemFree(savedGames);
  }
}

// ---------------------------------------------------------------------------
// DLL injection via LoadLibraryA remote thread
// ---------------------------------------------------------------------------
static bool InjectDLL(HANDLE hProcess, const char *dllPath) {
  SIZE_T pathLen = strlen(dllPath) + 1;
  LPVOID remotePath = VirtualAllocEx(hProcess, NULL, pathLen,
                                     MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!remotePath) return false;

  if (!WriteProcessMemory(hProcess, remotePath, dllPath, pathLen, NULL)) {
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
    return false;
  }

  HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
  LPVOID pLoadLibraryA = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
  if (!pLoadLibraryA) {
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
    return false;
  }

  HANDLE hThread = CreateRemoteThread(
      hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, remotePath, 0,
      NULL);
  if (!hThread) {
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
    return false;
  }

  WaitForSingleObject(hThread, 8000);
  DWORD exitCode = 0;
  GetExitCodeThread(hThread, &exitCode);

  CloseHandle(hThread);
  VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
  return (exitCode != 0);
}

// ---------------------------------------------------------------------------
// Find a running process by its executable name (case-insensitive)
// Returns PID or 0 if not found.
// ---------------------------------------------------------------------------
static DWORD FindProcessByName(const char *processName) {
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

  PROCESSENTRY32 pe = {sizeof(pe)};
  DWORD pid = 0;
  if (Process32First(hSnapshot, &pe)) {
    do {
      if (_stricmp(pe.szExeFile, processName) == 0) {
        pid = pe.th32ProcessID;
        break;
      }
    } while (Process32Next(hSnapshot, &pe));
  }
  CloseHandle(hSnapshot);
  return pid;
}

// ---------------------------------------------------------------------------
// Poll until a process with the given name appears (or timeout expires)
// ---------------------------------------------------------------------------
static DWORD WaitForProcessByName(const char *processName, DWORD timeoutMs) {
  DWORD elapsed = 0;
  while (elapsed < timeoutMs) {
    DWORD pid = FindProcessByName(processName);
    if (pid != 0) return pid;
    Sleep(200);
    elapsed += 200;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// Wine fallback: set locale via registry so child inherits Japanese codepage
// ---------------------------------------------------------------------------
static void SetJapaneseLocaleRegistry() {
  HKEY hKey = NULL;
  if (RegCreateKeyExA(HKEY_CURRENT_USER, "Control Panel\\International", 0,
                      NULL, 0, KEY_SET_VALUE, NULL, &hKey,
                      NULL) == ERROR_SUCCESS) {
    RegSetValueExA(hKey, "ACP",       0, REG_SZ, (const BYTE *)"932",      4);
    RegSetValueExA(hKey, "OEMCP",     0, REG_SZ, (const BYTE *)"932",      4);
    RegSetValueExA(hKey, "MACCP",     0, REG_SZ, (const BYTE *)"10001",    6);
    RegSetValueExA(hKey, "Locale",    0, REG_SZ, (const BYTE *)"00000411", 9);
    RegSetValueExA(hKey, "LocaleName",0, REG_SZ, (const BYTE *)"ja-JP",   6);
    RegCloseKey(hKey);
  }
  if (RegCreateKeyExA(HKEY_CURRENT_USER, "Control Panel\\Nls\\CodePage", 0,
                      NULL, 0, KEY_SET_VALUE, NULL, &hKey,
                      NULL) == ERROR_SUCCESS) {
    RegSetValueExA(hKey, "ACP",  0, REG_SZ, (const BYTE *)"932", 4);
    RegSetValueExA(hKey, "OEMCP",0, REG_SZ, (const BYTE *)"932", 4);
    RegCloseKey(hKey);
  }
}

// ---------------------------------------------------------------------------
// Build a Japanese environment block for Wine fallback
// ---------------------------------------------------------------------------
static char *BuildJapaneseEnvBlock() {
  LPCH envBlock = GetEnvironmentStringsA();
  if (!envBlock) return NULL;

  SIZE_T envSize = 0;
  {
    const char *p = envBlock;
    while (*p) { SIZE_T len = strlen(p) + 1; envSize += len; p += len; }
    envSize++;
  }

  const char *lcAll  = "LC_ALL=ja_JP.UTF-8";
  const char *lang   = "LANG=ja_JP.UTF-8";
  SIZE_T lcAllLen    = strlen(lcAll) + 1;
  SIZE_T langLen     = strlen(lang)  + 1;

  char *newEnv = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   envSize + lcAllLen + langLen + 64);
  if (!newEnv) { FreeEnvironmentStringsA(envBlock); return NULL; }

  char *dst = newEnv;
  const char *src = envBlock;
  while (*src) {
    SIZE_T varLen = strlen(src) + 1;
    if (_strnicmp(src, "LC_ALL=", 7) != 0 && _strnicmp(src, "LANG=", 5) != 0) {
      memcpy(dst, src, varLen); dst += varLen;
    }
    src += varLen;
  }
  memcpy(dst, lcAll, lcAllLen); dst += lcAllLen;
  memcpy(dst, lang,  langLen);  dst += langLen;
  *dst = '\0';

  FreeEnvironmentStringsA(envBlock);
  return newEnv;
}

// ---------------------------------------------------------------------------
// Auto-dismiss MessageBox by finding it and clicking "Yes"
// ---------------------------------------------------------------------------
struct EnumWindowsData {
  DWORD pid;
  bool clicked;
};

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  EnumWindowsData *data = (EnumWindowsData *)lParam;
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid == data->pid) {
    char className[256] = {0};
    GetClassNameA(hwnd, className, sizeof(className));
    if (strcmp(className, "#32770") == 0) {
      char title[256] = {0};
      GetWindowTextA(hwnd, title, sizeof(title));
      // Dialog title has "D.C.III"
      if (strstr(title, "D.C.III") || strstr(title, "D.C.") || strstr(title, "W.Y.")) {
        HWND hYes = GetDlgItem(hwnd, IDYES);
        if (hYes) {
          SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(IDYES, BN_CLICKED), (LPARAM)hYes);
          data->clicked = true;
        }
      }
    }
  }
  return TRUE;
}

// ---------------------------------------------------------------------------
// Extract an embedded resource to a file
// ---------------------------------------------------------------------------
static bool ExtractResourceToFile(int resourceId, const char* outPath) {
  HRSRC hResInfo = FindResourceA(NULL, MAKEINTRESOURCEA(resourceId), (LPCSTR)RT_RCDATA);
  if (!hResInfo) return false;
  HGLOBAL hResData = LoadResource(NULL, hResInfo);
  if (!hResData) return false;
  DWORD resSize = SizeofResource(NULL, hResInfo);
  void* pRes = LockResource(hResData);
  if (!pRes) return false;

  HANDLE hFile = CreateFileA(outPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return false;

  DWORD bytesWritten = 0;
  bool success = WriteFile(hFile, pRes, resSize, &bytesWritten, NULL) && (bytesWritten == resSize);
  CloseHandle(hFile);
  return success;
}

// ===========================================================================
// WinMain
// ===========================================================================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int) {

  // --- Game identity ---
#if defined(GAME_DC3WY)
  const char *launcherTitle = "DC3WY Patch Launcher";
  const char *gameExeName   = "DC3WY.EXE";
  const char *patchDllName  = "DC3WYPatch.dll";
#elif defined(GAME_DC4PH)
  const char *launcherTitle = "DC4PH Patch Launcher";
  const char *gameExeName   = "DC4PHDL.EXE";
  const char *patchDllName  = "DC4PHPatch.dll";
#else
  const char *launcherTitle = "DC3PP Patch Launcher";
  const char *gameExeName   = "DC3PP.EXE";
  const char *patchDllName  = "DC3PPPatch.dll";
#endif

  // --- Resolve launcher directory ---
  char currentDir[MAX_PATH] = {0};
  if (!BuildModuleDirectory(currentDir, sizeof(currentDir))) {
    MessageBoxA(NULL, "Failed to resolve launcher directory.",
                launcherTitle, MB_ICONERROR);
    return 1;
  }

  // --- DC3WY: ensure AdvData/Savegame exists ---
#if defined(GAME_DC3WY)
  EnsureSaveLayoutForDC3WY(currentDir);
#endif

  // --- Build full paths ---
  char exePath[MAX_PATH]  = {0};
  char dllPath[MAX_PATH]  = {0};
  sprintf_s(exePath, "%s\\%s", currentDir, gameExeName);
  sprintf_s(dllPath, "%s\\%s", currentDir, patchDllName);

  // --- Validate game EXE ---
  if (GetFileAttributesA(exePath) == INVALID_FILE_ATTRIBUTES) {
    char msg[512];
    sprintf_s(msg, "%s not found.\nPlace this launcher in the game folder.",
              gameExeName);
    MessageBoxA(NULL, msg, launcherTitle, MB_ICONERROR);
    return 1;
  }

  // --- Validate patch DLL ---
  if (GetFileAttributesA(dllPath) == INVALID_FILE_ATTRIBUTES) {
    char msg[512];
    sprintf_s(msg, "%s not found.\nPlace the patch DLL in the game folder.",
              patchDllName);
    MessageBoxA(NULL, msg, launcherTitle, MB_ICONERROR);
    return 1;
  }

// ===========================================================================
// DC3WY — Built-in Locale Emulator path (via embedded resources)
// ===========================================================================
#if defined(GAME_DC3WY)
  if (!IsRunningUnderWine()) {
    char tempPath[MAX_PATH] = {0};
    GetTempPathA(MAX_PATH, tempPath);
    
    char leDir[MAX_PATH] = {0};
    sprintf_s(leDir, "%sDC3WY_LE", tempPath);
    CreateDirectoryA(leDir, NULL);

    char leProcPath[MAX_PATH] = {0};
    sprintf_s(leProcPath, "%s\\LEProc.exe", leDir);

    // Extract resources
    ExtractResourceToFile(101, leProcPath);
    
    char tmpPath[MAX_PATH] = {0};
    sprintf_s(tmpPath, "%s\\LocaleEmulator.dll", leDir);
    ExtractResourceToFile(102, tmpPath);
    
    sprintf_s(tmpPath, "%s\\LoaderDll.dll", leDir);
    ExtractResourceToFile(103, tmpPath);
    
    sprintf_s(tmpPath, "%s\\LECommonLibrary.dll", leDir);
    ExtractResourceToFile(104, tmpPath);
    
    sprintf_s(tmpPath, "%s\\LEConfig.xml", leDir);
    ExtractResourceToFile(105, tmpPath);

    if (GetFileAttributesA(leProcPath) != INVALID_FILE_ATTRIBUTES) {
      // --- Build LEProc argument: "-runas GUID <full path to DC3WY.EXE>" ---
      char leArgs[MAX_PATH * 2] = {0};
      // GUID "10fd7363-085f-489e-a3f7-48d7cffa513a" is the default "Run in Japanese" profile in LEConfig.xml
      if (lpCmdLine && lpCmdLine[0] != '\0') {
        sprintf_s(leArgs, "-runas 10fd7363-085f-489e-a3f7-48d7cffa513a \"%s\" %s", exePath, lpCmdLine);
      } else {
        sprintf_s(leArgs, "-runas 10fd7363-085f-489e-a3f7-48d7cffa513a \"%s\"", exePath);
      }

      SHELLEXECUTEINFOA sei = {};
      sei.cbSize      = sizeof(sei);
      sei.fMask       = SEE_MASK_NOCLOSEPROCESS;
      sei.lpVerb      = "open";
      sei.lpFile      = leProcPath;
      sei.lpParameters= leArgs;
      sei.lpDirectory = currentDir;
      sei.nShow       = SW_SHOWDEFAULT;

      if (ShellExecuteExA(&sei)) {
        if (sei.hProcess) CloseHandle(sei.hProcess);
        Sleep(600);

        DWORD gamePid = WaitForProcessByName(gameExeName, 15000);
        if (gamePid != 0) {
          HANDLE hGame = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gamePid);
          if (hGame) {
            bool injected = InjectDLL(hGame, dllPath);
            CloseHandle(hGame);

            for (int i = 0; i < 15; i++) {
              EnumWindowsData data = { gamePid, false };
              EnumWindows(EnumWindowsProc, (LPARAM)&data);
              if (data.clicked) break;
              Sleep(200);
            }

            if (!injected) {
              MessageBoxA(NULL,
                "Patch DLL injection failed.\nGame started without patch.",
                launcherTitle, MB_ICONWARNING);
            }
          }
        } else {
          MessageBoxA(NULL,
            "Timed out waiting for game process.\nGame may have started without patch.",
            launcherTitle, MB_ICONWARNING);
        }
        return 0;
      }
    }
  }
#endif // GAME_DC3WY

// ===========================================================================
// Fallback: plain CreateProcess + DLL injection
// (also used for DC3PP / DC4PH, and for DC3WY under Wine or when LE absent)
// ===========================================================================

  char commandLine[MAX_PATH * 2] = {0};
  if (lpCmdLine && lpCmdLine[0] != '\0') {
    sprintf_s(commandLine, "\"%s\" %s", exePath, lpCmdLine);
  } else {
    sprintf_s(commandLine, "\"%s\"", exePath);
  }

  STARTUPINFOA siA = {};
  siA.cb = sizeof(siA);
  PROCESS_INFORMATION pi = {};

  BOOL created  = FALSE;
  char *customEnv = NULL;

  if (IsRunningUnderWine()) {
    SetJapaneseLocaleRegistry();
    customEnv = BuildJapaneseEnvBlock();
  }

  created = CreateProcessA(
      exePath,
      commandLine,
      NULL,
      NULL,
      FALSE,
      CREATE_SUSPENDED,
      customEnv,
      currentDir,
      &siA,
      &pi);

  if (customEnv) HeapFree(GetProcessHeap(), 0, customEnv);

  if (!created) {
    char msg[256];
    sprintf_s(msg, "Failed to start game. Error: %lu", GetLastError());
    MessageBoxA(NULL, msg, launcherTitle, MB_ICONERROR);
    return 1;
  }

  bool injected = InjectDLL(pi.hProcess, dllPath);
  ResumeThread(pi.hThread);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  if (!injected) {
    MessageBoxA(NULL,
                "Patch DLL injection failed.\nGame started without patch.",
                launcherTitle, MB_ICONWARNING);
    return 1;
  }

  return 0;
}
