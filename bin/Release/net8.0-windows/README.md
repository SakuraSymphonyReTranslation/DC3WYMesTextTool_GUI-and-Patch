# Da Capo 3 With You - MES Text Tool and Patch

A Windows GUI/CLI for exporting **Da Capo 3 With You** scripts from MES to JSON
and importing translations back into MES, together with the DC3WY patch and
custom launcher. The converter accepts DC3WY MES version `0xA09F` only.

## Download

Download both packages from [GitHub Releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest):

- [Released_patch.rar](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest/download/Released_patch.rar): patch, launcher, configuration, and installation instructions.
- [DC3WYMesTextTool_GUI.rar](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest/download/DC3WYMesTextTool_GUI.rar): MES/JSON converter and its required companion files.

The converter requires the **.NET 8 Desktop Runtime for Windows**. Keep its
EXE, DLL, `.deps.json`, and `.runtimeconfig.json` together after extraction.
The patch targets the 32-bit DC3WY game. Translation scripts are supplied separately.

## Folder names used in this guide

- `<GAME_DIR>`: your chosen game installation folder, containing `DC3WY.EXE` and `AdvData`.
- `<TOOL_DIR>`: the folder where you extracted `DC3WYMesTextTool_GUI.rar`. It can be anywhere.
- `<REPO_DIR>`: the folder where you cloned or extracted this source repository.

These are documentation placeholders, not special GitHub variables or literal
folder names. Replace them with your own paths; no particular drive letter is required.

## Install and launch the patch

1. Back up the game files that you will replace, including your current `DC3WYPatch.ini`.
2. Extract `Released_patch.rar` into `<GAME_DIR>` so that `DC3WYLauncher.exe` and `DC3WYPatch.dll` are beside `DC3WY.EXE`.
3. When updating, keep your customized INI or merge the new settings into it.
4. Run `<GAME_DIR>\DC3WYLauncher.exe` to start the game with the patch.
5. Press **F11** in-game to open **DC3WY Patch Settings**.

Launching `DC3WY.EXE` directly may not load the patch correctly. The custom
launcher embeds Locale Emulator and creates `AdvData\Savegame` when needed.

Translated MES files go in `<GAME_DIR>\id_Data\MES` for Indonesian or
`<GAME_DIR>\eng_data\MES` for English. Select the corresponding language in
the patch settings. Keep original Japanese MES files separately for importing.

## Word wrap and layout in v1.3

- **Default: 46 half-width columns**, shared by single import, batch import,
  and the GUI. ASCII and half-width kana count as one column; full-width
  characters count as two. The adjustable range is **10–100**.
- Dialogue and narration in story scripts named `wy_*.mes` wrap at word
  boundaries. Long words can be split, and breaks avoid opening/closing
  punctuation boundaries.
- **Four lines is a recommendation.** Longer messages produce warnings;
  text is not truncated or automatically split into separate dialogue pages.
- Existing manual newlines are preserved. CRLF and legacy literal `\n`
  are normalized, and repeated import does not keep adding line breaks.
- Nameplates, choices, auxiliary slots, and menu scripts such as `sce_*.mes`
  and `music.mes` are not automatically wrapped.
- Native ruby and font/color effects keep their native layout and are flagged
  for manual review in-game.
- Every successful import writes `<output.mes>.layout.json`, with the
  one-based JSON entry and MES slot associated with each warning.
- Export displays encrypted underscore placeholders as spaces. Import
  restores the underscores required by the encrypted MES format.
- Empty text, NUL, unsupported Shift-JIS characters, slot-count mismatches,
  and encoded text bodies above 259 bytes fail validation before a new MES is written.

The supplied patch configuration uses `DialogueLineSpacing=-10` and
`BacklogLineSpacing=-10`, giving normal body text a **24px line pitch**.
The DLL and INI control spacing; the converter only changes text and newlines.
Ruby and large-font text retain native spacing. If you keep an older INI,
merge these spacing values to use the current layout. Review dialogue and
backlog in-game, especially after increasing width or keeping manual line breaks.

## JSON format

Export produces UTF-8 JSON, grouping a nameplate and its following dialogue:

```json
[
  { "name": "Kiyotaka", "message": "Dialogue text." },
  { "message": "Narration text." }
]
```

MES opcode roles determine grouping, rather than quotation marks. Import
supports both this grouped format and the older flat format in which names
and dialogue are separate `{ "message": "..." }` entries. Expanded text-slot
counts must match the original MES, preventing translations from shifting
into the wrong dialogue or nameplate slots.

## Quick start: GUI

Open `<TOOL_DIR>\DC3WYMesTextTool_GUI.exe`. The window title is
**Da Capo 3 With You - MES Text Tool**.

### Single file export

1. Select the `Single File` tab.
2. In `Input MES/JSON`, select a `.mes` file.
3. Set `Output` to a `.json` path, or leave it empty to use the input filename with a `.json` extension.
4. Click `Export (MES -> JSON)` and check the log.

### Single file import

1. Select the translated `.json` in `Input MES/JSON`.
2. Put the matching **original Japanese MES**, with the same base filename, beside that JSON. The GUI uses it as the import template.
3. Set `Output` to a new `.mes` path.
4. Set `Word Wrap` to `46` unless you have checked another width in-game.
5. Click `Import (JSON -> MES)`, then review the log and `.mes.layout.json` report.

### Batch conversion

Use the `Batch` tab. For export, select the MES input folder in the MES folder
field. For import, select the translated JSON folder and the original
MES folder. Choose an output folder, set the wrap width, and run the conversion.
Files are matched by base filename, for example `wy_scene.json` and `wy_scene.mes`.
Missing original MES files are reported and skipped; check the log for failures.

## CLI tutorial

Open PowerShell in `<TOOL_DIR>`. The sample folders `original_mes`, `json`,
`mes_output`, and `json_grouped` below are relative to that working directory;
replace them and `wy_scene` with your own folder and file names. Use untouched
Japanese MES files in `original_mes` as the opcode/nameplate templates.
Create the `json` folder before using single-file export; batch export creates
its output folder automatically.

### Export one file

```powershell
.\DC3WYMesTextTool_GUI.exe export ".\original_mes\wy_scene.mes" ".\json\wy_scene.json"
```

If output is omitted, it defaults to the input filename with a `.json` extension.

### Import one file

```powershell
.\DC3WYMesTextTool_GUI.exe import ".\json\wy_scene.json" ".\original_mes\wy_scene.mes" ".\mes_output\wy_scene.mes" -w 46
```

The original MES argument is required. Output defaults to `output_<original filename>`
beside the original MES when omitted. `-w` is optional and defaults to `46`.

### Batch export

```powershell
.\DC3WYMesTextTool_GUI.exe batch-export ".\original_mes" ".\json"
```

The default output folder is `json_output` inside the input MES folder.

### Batch import

```powershell
.\DC3WYMesTextTool_GUI.exe batch-import ".\json" ".\original_mes" ".\mes_output" -w 46
```

The default output folder is `mes_output` inside the original MES folder.
Review each output's layout report before copying rebuilt MES files into
your selected translation folder under `<GAME_DIR>`.

### Restructure existing translation JSON

Convert legacy flat entries into grouped name/message entries using the
matching Japanese MES as the authoritative slot map:

```powershell
.\DC3WYMesTextTool_GUI.exe structure-json ".\json\wy_scene.json" ".\original_mes\wy_scene.mes" ".\json_grouped\wy_scene.json"
.\DC3WYMesTextTool_GUI.exe batch-structure-json ".\json" ".\original_mes" ".\json_grouped"
```

These commands preserve the expanded text-slot sequence and reject mismatched
slot counts. Omitting the output path/folder updates the input JSON in place.

## Drag and drop

- Drop a `.mes` file onto the converter EXE to export a `.json` beside it.
- Drop a folder onto the EXE to batch-export MES files into its `json_output` subfolder.

## Build from source

Clone with submodules so the patch's Detours dependency is available:

```powershell
git clone --recurse-submodules https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch.git
```

From `<REPO_DIR>`, build the GUI/CLI with the .NET 8 SDK:

```powershell
dotnet build DC3WYMesTextTool_GUI.csproj -c Release
```

For the patch, install Visual Studio 2022 with Desktop development with C++,
a Windows SDK, and CMake. From `<REPO_DIR>`, run:

```powershell
& ".\DC3WYPatch a\build_dc3wy_win32.bat"
```

The launcher uses the Locale Emulator files in `DC3WYPatch a/src/le_bin`.
See [patch build and verification notes](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/README.md).

| Output | Path relative to `<REPO_DIR>` |
| --- | --- |
| GUI/CLI executable | `bin/Release/net8.0-windows/DC3WYMesTextTool_GUI.exe` |
| Tool package | `bin/Release/net8.0-windows/DC3WYMesTextTool_GUI.rar` |
| Patch build | `DC3WYPatch a/build_dc3wy_win32/Release` |
| Ready-to-use patch files | `DC3WYPatch a/build/Released` |
| Patch package | `DC3WYPatch a/build/Released/Released_patch.rar` |

Build commands update binaries; repack the RAR packages separately after a
build. Generated caches, temporary build files, and Python virtual environments
are excluded from version control.

## Troubleshooting

- **Missing original MES:** use the matching original Japanese file, with the same base filename as the JSON.
- **Text does not fit:** inspect `.mes.layout.json`, shorten translations or adjust manual newlines, then check dialogue and backlog in-game.
- **Converter will not start:** install the .NET 8 Desktop Runtime and extract all converter companion files together.
- **Patch does not load:** run `DC3WYLauncher.exe` from `<GAME_DIR>` and keep the patch DLL beside it.
- **No output:** check the conversion log, output-folder permissions, and validation errors.

Detours, Locale Emulator, and the retained MesTextTool reference source have
their own licenses in their respective directories.
