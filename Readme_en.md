# DC3WY MES Text Tool — GUI and CLI

[Bahasa Indonesia](README.md) | **English**

An importer/exporter for **D.C.III With You**.

This distribution supports **Da Capo 3 With You** only (`dc3wy`, MES version `0xA09F`). The GUI, CLI, patch, and launcher are specific to this game. The main patch source is `DC3WYPatch a/src/dc3wypatch.cpp`.

## Downloads and game folder

Download both packages from [GitHub Releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest):

- **DC3WYMesTextTool_GUI.rar**: MES ↔ JSON importer/exporter with a GUI and CLI. Includes Indonesian and English documentation.
- **Released_patch.rar**: launcher, patch DLL, INI configuration, reference executable, and supporting files. Includes installation instructions in both languages.

`<GAME_DIR>` means **your game's installation folder**, which contains `DC3WY.EXE` and `AdvData`. It is a documentation placeholder: replace it with your actual game folder, without the `<` and `>` characters. No particular drive or installation path is required.

1. Close the game and back up any files that will be replaced, especially the executable and `DC3WYPatch.ini`.
2. Extract the contents of `Released_patch.rar` directly into `<GAME_DIR>`, so that `DC3WYLauncher.exe`, `DC3WYPatch.dll`, and `DC3WYPatch.ini` are next to `DC3WY.EXE`.
3. When updating the patch, preserve your customized INI or merge the new settings into it.
4. Run **`<GAME_DIR>\DC3WYLauncher.exe`** with `<GAME_DIR>` as the working directory. If you use a shortcut, set its **Start in** field to that folder.
5. Press **F11** to open **DC3WY Patch Settings** and select a language. Place Indonesian MES files in `<GAME_DIR>\id_Data\MES`; for English, use `<GAME_DIR>\eng_data\MES`.

```text
<GAME_DIR>/
├── DC3WY.EXE
├── DC3WYLauncher.exe
├── DC3WYPatch.dll
├── DC3WYPatch.ini
├── IconGroup103.ico
├── scenario_titles.ini
├── AdvData/
└── id_Data/
    └── MES/
```

The two archives provide the tool and patch runtime; translated scripts are installed separately. The launcher embeds Locale Emulator and creates `AdvData\Savegame` when needed. Use the launcher to load the patch correctly.

The repository's source folder can be placed anywhere. The patch package is also available under `DC3WYPatch a/build/Released` in the source tree.

## Opening the application

Extract `DC3WYMesTextTool_GUI.rar` to a folder of your choice and run `DC3WYMesTextTool_GUI.exe`. If you are using the source repository, run `bin\Release\net8.0-windows\DC3WYMesTextTool_GUI.exe`. The ready-to-use package is in the same folder.

The application requires the **.NET 8 Desktop Runtime for Windows**. Keep the EXE, DLL, `.deps.json`, and `.runtimeconfig.json` together. Its window title is **Da Capo 3 With You - MES Text Tool**.

## Single File

**MES → JSON (export)**

1. Set **Input MES/JSON** to the original MES file.
2. Set **Output**, or leave it empty to create the JSON file next to the input.
3. Click **Export (MES -> JSON)** and check the log.

Export combines character names and dialogue according to the DC3WY opcodes:

```json
[
  { "message": "Narration." },
  { "name": "Kiyotaka", "message": "「Character dialogue.」" }
]
```

Single File, Batch, and the CLI use this combined format. Grouping follows the name/dialogue positions in the MES file, rather than quotation marks. Export does not translate names. Encrypted underscore placeholders are displayed as spaces and restored on import.

**JSON → MES (import)**

1. Set **Input MES/JSON** to the translated JSON file.
2. Place the **original Japanese MES** for the same scenario next to the JSON, with the same base name: `wy_scene.json` uses `wy_scene.mes`.
3. Choose an **Output** file different from the original MES. If left empty, the GUI creates `new_<original MES filename>` in the template's folder.
4. Set **Word Wrap**: the default is `46` in Single File, Batch, and the CLI; the supported range is **10–100 half-width columns**.
5. Click **Import (JSON -> MES)**, then review the log and `<output.mes>.layout.json` report.

The Single File GUI looks for the template next to the JSON. To keep templates in a different folder, use Batch or the CLI, which accept a separate original MES folder/path.

The importer accepts combined JSON and the legacy format that stores names and dialogue as separate `message` entries. The expanded text-slot count must match the original MES; do not add, remove, or reorder entries. The original template is required because JSON does not contain all instructions, labels, assets, or scenario flow.

## Batch

- **Export:** set **MES Folder** and **Output Folder**, then click **Batch Export**.
- **Import:** set **JSON Folder**, the original **MES Folder**, and a separate **Output Folder**, then click **Batch Import**.
- File base names must match: `wy_scene.json` uses `wy_scene.mes` as its template.
- Start with wrap `46`, then review the log and each imported file's layout report.
- Missing templates are reported and skipped. Check the complete log for files that were not processed successfully.

## Layout and backlog icons

Word wrapping applies to ordinary dialogue/narration in `wy_*.mes` story scripts. Character names, choices, auxiliary slots, and menu scripts such as `sce_*.mes` and `music.mes` are not wrapped automatically. Ruby and font/color effects retain their native layout and are flagged for review in the game.

Wrapping counts ASCII characters and half-width katakana as one column, and full-width characters as two columns. Manual line breaks are preserved; CRLF and literal `\n` forms are normalized. Words longer than the limit can be split, with adjustments at punctuation boundaries.

**Four lines is a recommendation.** Longer messages are recorded in `.mes.layout.json`, with one-based JSON entry and MES slot numbers. Text is not truncated or automatically split into new dialogue pages. A report is written even when there are no warnings.

**`DC3WYPatch.dll` handles vertical spacing and icon selection at runtime.** The importer controls text and line breaks. See the [English patch README](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/Readme_en.md) for technical details.

The patch package uses `DialogueLineSpacing=-10`, `BacklogLineSpacing=-10`, and `ShowBacklogIcon=1`. Normal text has a **24 px** line pitch, placing four lines at `[0, 24, 48, 72]` px. Icon lookup supports **25 translated-name aliases**, including underscore forms handled by the patch. Ruby and large-font text retain native spacing.

Wrap `46` controls text width during import; DLL spacing controls vertical positions while the game is running. Wrapping does not guarantee that every message will fit within four lines. If you keep an older INI, merge the spacing values above. Review dialogue and backlog after import, especially when changing the width or retaining many manual line breaks.

## Validation and encoding

- The converter accepts DC3WY MES version `0xA09F` only.
- The JSON text-slot count must match the MES template. Name/dialogue grouping follows the original opcode roles.
- Empty or whitespace-only text, NUL, and characters that cannot be encoded in Shift-JIS/CP932 are rejected before a new MES is written.
- Dialogue bodies are limited to **259 encoded bytes**, matching the DC3WY renderer's buffer limit. Shorten the translation if this limit is exceeded.
- Unsupported characters cause an error instead of silently becoming `?`.

## CLI

Run these examples from the executable's folder. Replace the file/folder names with your own; create the `json` folder before single-file export.

```powershell
.\DC3WYMesTextTool_GUI.exe export "original_mes\wy_scene.mes" "json\wy_scene.json"
.\DC3WYMesTextTool_GUI.exe import "json\wy_scene.json" "original_mes\wy_scene.mes" "mes_output\wy_scene.mes" -w 46
.\DC3WYMesTextTool_GUI.exe batch-export "original_mes" "json"
.\DC3WYMesTextTool_GUI.exe batch-import "json" "original_mes" "mes_output" -w 46
```

`-w` is optional and defaults to `46`, with a range of **10–100**. If output is omitted, single-file export writes JSON next to the input; CLI import writes `output_<original MES filename>` next to the template. Batch output defaults to `json_output` inside the input MES folder for export, or `mes_output` inside the original MES folder for import.

To restructure legacy JSON into the combined name/dialogue format, use the original Japanese MES as the reference:

```powershell
.\DC3WYMesTextTool_GUI.exe structure-json "json\wy_scene.json" "original_mes\wy_scene.mes" "json_grouped\wy_scene.json"
.\DC3WYMesTextTool_GUI.exe batch-structure-json "json" "original_mes" "json_grouped"
```

Both commands preserve the text-slot sequence and reject mismatched slot counts. **If output is omitted, the input JSON is updated in place.** Use a separate output folder to keep the original copy.

Drag an MES file onto the EXE to export JSON next to it. Drop a folder containing MES files to export into its `json_output` subfolder.

## Verification results

The builds and binaries used by this documentation were previously checked:

- The .NET Release build passed with no warnings or errors; the x86 patch and launcher Release build passed.
- A 93-entry scene passed export → import → re-export → re-import, including name grouping, a layout report, and byte-identical repeated import.
- The DLL hooks passed 56 line-spacing cases and checks of 161 names from JSON with 25 supported aliases, including register/stack preservation.

This documentation update preserves those binaries. The figures above come from earlier verification; the README changes do not include a new interactive game test.

## Building and testing

Clone with submodules so that the Detours dependency is available:

```powershell
git clone --recurse-submodules https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch.git
```

From the project's source folder, run:

```powershell
dotnet build DC3WYMesTextTool_GUI.csproj -c Release
& ".\DC3WYPatch a\build_dc3wy_win32.bat"
```

Building the GUI requires the **.NET 8 SDK**. Building the patch requires **Visual Studio 2022 with Desktop development with C++**, a Windows SDK, CMake, Detours, and the Locale Emulator files in `DC3WYPatch a/src/le_bin`.

| Output | Path relative to the source folder |
| --- | --- |
| GUI/CLI and tool package | `bin/Release/net8.0-windows` |
| Ready-to-use patch files and RAR | `DC3WYPatch a/build/Released` |

The build script copies the DLL and launcher into `build/Released` and preserves the existing INI. Repack the RAR archives separately after building. Hook verifier instructions are in the [English patch README](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/Readme_en.md); translation audit data is supplied separately.

## Troubleshooting

- **Original MES not found:** check that its base name matches the JSON. For Single File, keep both files in the same folder.
- **Text does not fit:** inspect `.mes.layout.json`, shorten translations or adjust line breaks, then review dialogue and backlog in the game.
- **Application will not start:** install the .NET 8 Desktop Runtime and extract all companion tool files together.
- **Patch does not load:** run `DC3WYLauncher.exe` from `<GAME_DIR>` and keep the DLL next to it.
- **No output:** check the log, output-folder permissions, and validation messages.

Detours, Locale Emulator, and the MesTextTool reference source have their own licenses in their respective directories.
