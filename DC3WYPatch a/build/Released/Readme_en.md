# DC3WY Patch — installation guide

[Bahasa Indonesia](README.md) | **English**

Launcher and runtime patch for the 32-bit version of **Da Capo 3 With You**. Translated scripts are installed separately.

## Installation

`<GAME_DIR>` means **your game's installation folder**, containing `DC3WY.EXE` and `AdvData`. Replace this placeholder with the actual path, without the `<` and `>` characters; no particular drive is required.

1. Close the game and back up any files that will be replaced, especially the executable and `DC3WYPatch.ini`.
2. Extract all contents of `Released_patch.rar` directly into `<GAME_DIR>`, so that `DC3WYLauncher.exe`, `DC3WYPatch.dll`, and `DC3WYPatch.ini` are next to `DC3WY.EXE`.
3. When updating, preserve your customized INI or merge the new settings into it.
4. Run **`<GAME_DIR>\DC3WYLauncher.exe`** with `<GAME_DIR>` as the working directory. For shortcuts, set **Start in** to that folder.
5. Press **F11** to open **DC3WY Patch Settings**. Select a language, then use Indonesian MES files in `<GAME_DIR>\id_Data\MES` or English MES files in `<GAME_DIR>\eng_data\MES`.

The launcher embeds Locale Emulator and creates `AdvData\Savegame` when needed. Use the launcher to load the patch DLL correctly.

## Package contents

- `DC3WY.EXE`: reference game executable.
- `DC3WYLauncher.exe`: patch launcher.
- `DC3WYPatch.dll` and `DC3WYPatch.ini`: patch runtime and settings.
- `IconGroup103.ico` and `scenario_titles.ini`: supporting files.
- `README.md` and `Readme_en.md`: Indonesian and English instructions.

## Layout and backlog icons

Import translations with **DC3WYMesTextTool_GUI**, using the default of **46 half-width columns** and **four recommended lines**. Imports write `.mes.layout.json` reports; long messages are reported without truncating text. Manual line breaks are preserved.

The package uses `DialogueLineSpacing=-10`, `BacklogLineSpacing=-10`, `AdvancedSettings=1`, `DisableBacklogSpacing=0`, and `ShowBacklogIcon=1`. Normal text has a **24 px** line pitch, placing four lines at `[0, 24, 48, 72]` px. Icon lookup supports 25 translated-name aliases. Merge these settings if you keep an older INI.

Wrapping controls width during import; the DLL controls vertical spacing and icons at runtime. Four lines are not guaranteed for every message. Ruby and large-font text retain native spacing; review the results in dialogue and backlog.

The conversion tool requires the **.NET 8 Desktop Runtime for Windows**. The patch launcher is a separate native application.

## Full guides

- [English tool guide](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/Readme_en.md)
- [English patch and build guide](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/Readme_en.md)
- [Latest release](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest)

Detours and Locale Emulator have their own licenses in the repository's source directories.
