# Da Capo 3 With You patch

This project builds only `DC3WYPatch.dll` and `DC3WYLauncher.exe` for the
32-bit `DC3WY.EXE` game. The patch source is `src/dc3wypatch.cpp`.

## Build

Install Visual Studio 2022 with Desktop development with C++, a Windows SDK,
and CMake. Clone the repository with `--recurse-submodules` (or run
`git submodule update --init --recursive` from the repository root) to obtain
Detours. Keep the Locale Emulator files in `src/le_bin`.
Run `build_dc3wy_win32.bat` from any working directory.

Equivalent commands from this folder:

```powershell
cmake -S . -B build_dc3wy_win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build_dc3wy_win32 --config Release --target DC3WYPatch DC3WYLauncher
```

The batch script copies the compiled DLL and launcher into `build/Released`.
It preserves existing INI settings. The launcher embeds Locale Emulator.

## Use

`<GAME_DIR>` means the player's chosen installation folder containing
`DC3WY.EXE` and `AdvData`; replace this placeholder with your own path.
Copy the contents of `build/Released` into `<GAME_DIR>`, then
run `<GAME_DIR>\DC3WYLauncher.exe`. Keep your existing `DC3WYPatch.ini`
when updating. Press F11 in-game to open DC3WY Patch Settings.

The approved configuration uses `DialogueLineSpacing=-10` and
`BacklogLineSpacing=-10`, giving normal body text a 24px line pitch.
Native ruby and large-font text retain their native spacing. The name-icon
fallback supports translated DC3WY character names and underscore aliases.

For MES/JSON export and import, use `../bin/Release/net8.0-windows/DC3WYMesTextTool_GUI.exe`.
The converter defaults to 46 half-width columns and produces layout reports.

## Verification

With Python packages `pefile`, `capstone` and `unicorn` installed:

```powershell
python tools/verify_line_spacing.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll"
python tools/verify_backlog.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll" "<NAME_AUDIT_JSON>"
```

`<NAME_AUDIT_JSON>` is a translation-specific audit file with `names` and
`length_mismatches` fields, supplied separately for the backlog check.
These checks execute the compiled x86 hooks against the original DC3WY code.
They check body spacing, register/stack preservation and translated name icons.
Detours and Locale Emulator remain third-party dependencies with their own licenses.
