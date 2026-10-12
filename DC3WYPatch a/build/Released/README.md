# Da Capo 3 With You patch - v1.3

`<GAME_DIR>` means your chosen game installation folder: the folder containing
`DC3WY.EXE` and `AdvData`. Replace this placeholder with your actual path.

## Installation

1. Back up the game files that you will replace, including your current `DC3WYPatch.ini`.
2. Extract this archive into `<GAME_DIR>`. Keep `DC3WYLauncher.exe`, `DC3WYPatch.dll`, and `DC3WYPatch.ini` beside `DC3WY.EXE`.
3. When updating, preserve your customized INI or merge the new settings into it.
4. Start the game with `<GAME_DIR>\DC3WYLauncher.exe`.
5. Press **F11** in-game to open **DC3WY Patch Settings**.

Use the custom launcher so the patch loads correctly. It embeds Locale Emulator
and creates `AdvData\Savegame` when needed. This package targets the 32-bit DC3WY
game; translation scripts are supplied separately.

Place Indonesian MES files in `<GAME_DIR>\id_Data\MES`, or English MES files
in `<GAME_DIR>\eng_data\MES`, and select the corresponding language in F11 settings.

## Text layout

Use the matching `DC3WYMesTextTool_GUI.rar` converter to import translations.
The default is **46 half-width columns** for normal story dialogue/narration,
with **four lines recommended**. Imports write a `.mes.layout.json` report.
Longer messages are reported without truncation; manual newlines are preserved.

The supplied INI uses `DialogueLineSpacing=-10` and `BacklogLineSpacing=-10`,
giving normal body text a **24px line pitch**. If you retain an older INI,
merge these values to use this layout. Ruby and large-font messages retain
native spacing and should be checked in-game, along with the backlog.

The converter requires the .NET 8 Desktop Runtime for Windows. The patch
launcher is a separate native application.

## Source and full instructions

- [Repository and converter guide](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch)
- [Latest releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest)

Detours and Locale Emulator are third-party dependencies with licenses in
their respective source directories in the repository.
