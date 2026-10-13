# DC3WY Patch — installation and build

[Bahasa Indonesia](README.md) | **English**

This patch and launcher support the 32-bit version of **Da Capo 3 With You** only. The main source file is `src/dc3wypatch.cpp`.

## Installation

Download `Released_patch.rar` from [Releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest), or use the package in `build/Released`.

`<GAME_DIR>` means your game's installation folder, containing `DC3WY.EXE` and `AdvData`. Replace this placeholder with your actual game folder, without the `<` and `>` characters; no particular drive is required.

1. Close the game and back up any files that will be replaced, especially `DC3WY.EXE` and `DC3WYPatch.ini`.
2. Extract the package contents directly into `<GAME_DIR>`. Keep the launcher and patch DLL next to the game executable.
3. When updating, preserve your customized INI or merge the new settings into it.
4. Run **`<GAME_DIR>\DC3WYLauncher.exe`** with `<GAME_DIR>` as the working directory; shortcuts must use that folder in **Start in**.
5. Press **F11** to open **DC3WY Patch Settings** and select the appropriate language.

The launcher embeds Locale Emulator and creates `AdvData\Savegame` when needed. Use the launcher to load the patch DLL correctly.

The package contains `DC3WY.EXE`, `DC3WYLauncher.exe`, `DC3WYPatch.dll`, `DC3WYPatch.ini`, `IconGroup103.ico`, `scenario_titles.ini`, and Indonesian and English README files. Install translated scripts separately under `<GAME_DIR>\id_Data\MES` or `<GAME_DIR>\eng_data\MES`, according to the language selected in the patch settings.

## Line spacing and backlog icons

- `DialogueLineSpacing=-10` and `BacklogLineSpacing=-10` produce a normal **24 px** line pitch, placing four lines at `[0, 24, 48, 72]` px.
- The package uses `AdvancedSettings=1`, `DisableBacklogSpacing=0`, and `ShowBacklogIcon=1`. If you keep an older INI, merge these settings to use the packaged layout.
- Ruby and large-font text retain native spacing and need to be reviewed in the game.
- Icon lookup supports **25 translated-name aliases**, including underscore forms handled by the patch, and then uses the game's native lookup.

The MES importer handles word wrapping, with a default of **46 half-width columns** and **four recommended lines**. The DLL controls vertical spacing and icons. Wrapping does not guarantee that every message fits within four lines; older scripts that have not been reimported need to be reviewed in dialogue and backlog. See the [English tool guide](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/Readme_en.md) for the import workflow and `.mes.layout.json` reports.

## Build (Visual Studio 2022, Win32)

Install Visual Studio 2022 with **Desktop development with C++**, a Windows SDK, and CMake. Clone the repository with `--recurse-submodules`, or run `git submodule update --init --recursive` from the repository root to obtain Detours. Keep the Locale Emulator files in `src/le_bin`.

Run `build_dc3wy_win32.bat`; the script changes to its own folder. Equivalent commands from the `DC3WYPatch a` folder:

```powershell
cmake -S . -B build_dc3wy_win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build_dc3wy_win32 --config Release --target DC3WYPatch DC3WYLauncher
```

A manual build produces `build_dc3wy_win32/Release/DC3WYPatch.dll` and `DC3WYLauncher.exe`. The BAT script also copies both files to `build/Released` and preserves the existing INI. Use the configuration and supporting files from `build/Released` when installing a new build. Repack the RAR archive separately after building.

## Hooks and verification

Earlier verification of the packaged DLL passed **56 line-spacing cases** and checks of **161 JSON names with 25 supported aliases**, including register/stack preservation. This documentation update preserves those binaries and does not include a new interactive game test.

To run the verifiers from the `DC3WYPatch a` folder:

```powershell
python -X utf8 tools/verify_line_spacing.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll"
python -X utf8 tools/verify_backlog.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll" "<NAME_AUDIT_JSON>"
```

Replace the placeholders before running the commands. The verifiers require Python with `pefile`, `capstone`, and `unicorn`. `<NAME_AUDIT_JSON>` is a separately supplied translation audit file with `names` and `length_mismatches` fields. These checks execute the compiled x86 hooks against the original DC3WY code to check line spacing, register/stack preservation, and translated-name icons.

Detours and Locale Emulator are third-party dependencies with their own licenses in their source directories.
