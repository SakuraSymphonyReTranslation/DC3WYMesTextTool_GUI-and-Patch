# DC3WY MES Text Tool (GUI + CLI)

A Windows tool for converting **Da Capo 3 With You** script files between:

- `MES -> JSON` (export)
- `JSON -> MES` (import)

It supports both:

- Single file conversion
- Batch conversion (entire folder)

## Tool Location

Main executable folder:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\bin\Release\net8.0-windows`

Packed GUI tool:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\bin\Release\net8.0-windows\DC3WYMesTextTool_GUI.rar`

## Output Build Locations (Patch + Launcher)

Patch and launcher build output:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build`

Ready-to-use release output:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build\Released`

## Important: Custom Launcher Required

To use this patch correctly, launch the game using:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build\Released\DC3WYLauncher.exe`

Running the game directly from `DC3WY.EXE` may not load the patch correctly.

## Features

- GUI mode (run without arguments)
- CLI mode (run commands from terminal)
- Drag-and-drop support for `.mes` files and folders
- Shift-JIS support for game script text

## JSON Format

Exported JSON is an array of entries like:

```json
[
  { "name": "Character Name", "message": "Dialogue line" },
  { "message": "Narration line" }
]
```

## Quick Start (GUI)

1. Open `DC3WYMesTextTool_GUI.exe`.
2. Use the `Single File` tab for one file.
3. Use the `Batch` tab for folder-based conversion.
4. Check the log panel at the bottom for success/error details.

### Single File: Export (MES -> JSON)

1. In `Input MES/JSON`, choose a `.mes` file.
2. In `Output JSON/MES`, set output `.json` path (optional).
3. Click `Export (MES -> JSON)`.

### Single File: Import (JSON -> MES)

Current GUI behavior:

- The GUI auto-detects original MES by replacing `.json` with `.mes` in the same folder.
- If matching `.mes` is not found, import will fail.

Steps:

1. In `Input MES/JSON`, choose a `.json` file.
2. Make sure the original `.mes` with the same base filename is in the same folder.
3. In `Output JSON/MES`, set output `.mes` path (optional).
4. Set `Word Wrap Width` (default: `67`).
5. Click `Import (JSON -> MES)`.

## CLI Tutorial

Open terminal in:

- `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\bin\Release\net8.0-windows`

Run:

- `.\DC3WYMesTextTool_GUI.exe <command> ...`

### 1) Export Single File (MES -> JSON)

```powershell
.\DC3WYMesTextTool_GUI.exe export "H:\path\script.mes" "H:\path\script.json"
```

If output path is omitted, output defaults to the same name with `.json`.

### 2) Import Single File (JSON -> MES)

```powershell
.\DC3WYMesTextTool_GUI.exe import "H:\path\script.json" "H:\path\script.mes" "H:\path\script_new.mes"
```

Notes:

- The second argument (`original.mes`) is required.
- For single CLI import, internal default wrap width is `28`.

### 3) Batch Export (Folder) (MES -> JSON)

```powershell
.\DC3WYMesTextTool_GUI.exe batch-export "H:\path\mes_folder" "H:\path\json_output"
```

If output folder is omitted, defaults to `mes_folder\json_output`.

### 4) Batch Import (Folder) (JSON -> MES)

```powershell
.\DC3WYMesTextTool_GUI.exe batch-import "H:\path\json_folder" "H:\path\mes_folder" "H:\path\mes_output" -w 67
```

Arguments:

- `json_folder`: translated `.json` files
- `mes_folder`: original `.mes` files used as base
- `mes_output`: output folder for rebuilt `.mes`
- `-w`: wrap width (optional, default `67`)

Matching is done by filename:

- `scene01.json` -> uses `scene01.mes`

If original MES file is missing, that JSON file is skipped.

## Drag & Drop

- Drop a `.mes` file onto the EXE: auto export to `.json` with same filename.
- Drop a folder onto the EXE: auto batch export all `.mes` files in that folder.

## Troubleshooting

- `original MES not found`: ensure matching `.mes` exists for each `.json`.
- Import output looks broken: try different wrap width for batch import (`-w`).
- No output file: check write permission for output folder.

## Build & Release Summary

- Active build folder: `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build`
- Final ready release folder: `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build\Released`
- Release package: `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\DC3WYPatch a\build\Released\Released_patch.rar`
- Tool package: `H:\Games\DC3WY\DC3WYMesTextTool_GUI and Patch\bin\Release\net8.0-windows\DC3WYMesTextTool_GUI.rar`

## Release Description (v1.0.0)

Initial public release.
Includes:
Released_patch.rar
DC3WYMesTextTool_GUI.rar
Important: Use the custom DC3WYLauncher from build/Released to run the patch properly.

The file doesn't contain any viruses; if your antivirus somehow detect it as anomaly.. it's just a false alarm. Trust me, I made this patch for everyone who wants to create or modify the Da Capo 3 With You patch. I have no intention of harming anyone.
