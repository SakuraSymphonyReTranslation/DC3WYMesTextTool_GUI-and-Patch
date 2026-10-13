# DC3WY MES Text Tool — GUI dan CLI

**Bahasa Indonesia** | [English](Readme_en.md)

Importer/exporter untuk **D.C.III With You**.

Distribusi ini hanya mendukung **Da Capo 3 With You** (`dc3wy`, versi MES `0xA09F`). GUI, CLI, patch, dan launcher dikhususkan untuk game ini. Source utama patch: `DC3WYPatch a/src/dc3wypatch.cpp`.

## Unduhan dan folder instalasi

Unduh kedua paket terbaru dari [GitHub Releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest):

- **DC3WYMesTextTool_GUI.rar**: importer/exporter MES ↔ JSON, GUI dan CLI. Termasuk dokumentasi bahasa Indonesia dan Inggris.
- **Released_patch.rar**: launcher, DLL patch, INI, executable referensi, dan berkas pendukung. Termasuk petunjuk instalasi dalam dua bahasa.

`<GAME_DIR>` berarti **folder utama instalasi game pilihan pemain**, yaitu folder yang berisi `DC3WY.EXE` dan `AdvData`. Ini placeholder dokumentasi: ganti dengan lokasi game Anda, tanpa tanda `<` dan `>`. Tidak ada drive atau lokasi instalasi wajib.

1. Tutup game dan simpan salinan berkas lama yang akan diganti, terutama executable dan `DC3WYPatch.ini`.
2. Ekstrak isi `Released_patch.rar` langsung ke `<GAME_DIR>`, sehingga `DC3WYLauncher.exe`, `DC3WYPatch.dll`, dan `DC3WYPatch.ini` berada di sebelah `DC3WY.EXE`.
3. Jika memperbarui patch, pertahankan INI yang sudah disesuaikan atau gabungkan pengaturan baru ke dalamnya.
4. Jalankan **`<GAME_DIR>\DC3WYLauncher.exe`** dengan working directory `<GAME_DIR>`. Jika memakai shortcut, isi **Start in** dengan folder tersebut.
5. Tekan **F11** untuk membuka **DC3WY Patch Settings** dan pilih bahasa. Berkas MES Indonesia ditempatkan di `<GAME_DIR>\id_Data\MES`; untuk bahasa Inggris, gunakan `<GAME_DIR>\eng_data\MES`.

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

Kedua RAR menyediakan tool dan runtime patch; naskah terjemahan dipasang terpisah. Launcher menyertakan Locale Emulator dan membuat `AdvData\Savegame` bila diperlukan. Gunakan launcher agar patch dimuat dengan benar.

Folder source repositori boleh ditempatkan di mana saja. Paket patch juga tersedia di `DC3WYPatch a/build/Released` pada source.

## Buka aplikasi

Ekstrak `DC3WYMesTextTool_GUI.rar` ke folder pilihan Anda dan jalankan `DC3WYMesTextTool_GUI.exe`. Jika memakai source repositori, jalankan `bin\Release\net8.0-windows\DC3WYMesTextTool_GUI.exe`. Paket siap pakai berada di folder yang sama.

Aplikasi membutuhkan **.NET 8 Desktop Runtime untuk Windows**. Simpan EXE, DLL, `.deps.json`, dan `.runtimeconfig.json` dalam satu folder. Judul jendelanya adalah **Da Capo 3 With You - MES Text Tool**.

## Single File

**MES → JSON (exporter)**

1. Isi **Input MES/JSON** dengan MES asli.
2. Isi **Output**, atau kosongkan untuk membuat JSON di sebelah input.
3. Klik **Export (MES -> JSON)** dan periksa log.

Hasil ekspor menggabungkan nama karakter dan dialog berdasarkan opcode DC3WY:

```json
[
  { "message": "Narasi." },
  { "name": "Kiyotaka", "message": "「Dialog karakter.」" }
]
```

Single File, Batch, dan CLI menggunakan format gabungan tersebut. Pengelompokan mengikuti posisi nama/dialog dalam MES, bukan tanda kutip. Ekspor tidak menerjemahkan nama. Placeholder underscore pada teks terenkripsi ditampilkan sebagai spasi dan dipulihkan saat impor.

**JSON → MES (importer)**

1. Isi **Input MES/JSON** dengan JSON terjemahan.
2. Letakkan **MES Jepang asli** untuk skenario yang sama di sebelah JSON, dengan nama dasar yang sama: `wy_scene.json` memakai `wy_scene.mes`.
3. Tentukan file **Output** yang berbeda dari MES asli. Jika kosong, GUI membuat `new_<nama MES asli>` di folder template.
4. Pilih **Word Wrap**: default `46` pada Single File, Batch, dan CLI; rentang yang didukung **10–100 kolom setengah lebar**.
5. Klik **Import (JSON -> MES)**, lalu periksa log dan laporan `<output.mes>.layout.json`.

GUI Single File mencari template di sebelah JSON. Untuk menyimpan template di folder lain, gunakan Batch atau CLI yang menyediakan argumen folder/path MES asli.

Importer menerima JSON gabungan dan format lama yang memisahkan nama serta dialog menjadi entri `message`. Jumlah slot teks setelah JSON diperluas harus sama dengan MES asli; jangan menambah, menghapus, atau mengubah urutan entri. Template asli dibutuhkan karena JSON tidak memuat seluruh instruksi, label, aset, dan alur skenario.

## Batch

- **Export:** isi **MES Folder** dan **Output Folder**, lalu klik **Batch Export**.
- **Import:** isi **JSON Folder**, **MES Folder** asli, dan **Output Folder** yang berbeda, lalu klik **Batch Import**.
- Nama file harus sama: `wy_scene.json` memakai template `wy_scene.mes`.
- Pilih wrap `46` sebagai titik awal, lalu periksa log dan laporan layout setiap hasil impor.
- Template yang hilang dilaporkan dan dilewati. Periksa seluruh log untuk mengetahui file yang belum berhasil diproses.

## Layout dan ikon backlog

Word wrap diterapkan pada dialog/narasi biasa di script cerita `wy_*.mes`. Nama karakter, pilihan, slot tambahan, dan script menu seperti `sce_*.mes` serta `music.mes` tidak dibungkus otomatis. Ruby dan efek font/warna mempertahankan layout native dan ditandai untuk ditinjau di game.

Wrap memakai satu kolom untuk ASCII/katakana setengah lebar dan dua kolom untuk karakter lebar penuh. Pemisah baris manual dipertahankan; CRLF dan bentuk literal `\n` dinormalisasi. Kata yang lebih panjang dari batas dapat dipotong, dengan penyesuaian posisi tanda baca.

**Empat baris adalah rekomendasi.** Pesan yang lebih panjang dicatat dalam laporan `.mes.layout.json`, lengkap dengan nomor entri JSON dan slot MES mulai dari 1. Teks tidak dipotong atau dibagi otomatis menjadi halaman dialog baru. Laporan tetap ditulis saat tidak ada peringatan.

**Jarak vertikal dan pemilihan gambar ikon ditangani `DC3WYPatch.dll` saat runtime.** Importer mengatur teks dan pemisah baris. Petunjuk teknis tersedia di [README patch](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/README.md).

Paket patch memakai `DialogueLineSpacing=-10`, `BacklogLineSpacing=-10`, dan `ShowBacklogIcon=1`. Pitch teks normal adalah **24 px**, sehingga empat baris berada pada `[0, 24, 48, 72]` px. Lookup ikon mendukung **25 alias nama terjemahan**, termasuk bentuk underscore yang ditangani patch. Ruby dan font besar mempertahankan jarak native.

Wrap `46` mengatur lebar teks saat impor; spacing DLL mengatur jarak vertikal saat game berjalan. Wrap tidak menjamin semua pesan muat dalam empat baris. Jika mempertahankan INI lama, gabungkan nilai spacing di atas. Periksa dialog dan backlog setelah impor, terutama bila mengubah lebar atau mempertahankan banyak pemisah baris manual.

## Validasi dan encoding

- Converter hanya menerima MES DC3WY versi `0xA09F`.
- Jumlah slot JSON harus sesuai dengan template MES. Pengelompokan nama/dialog mengikuti peran opcode asli.
- Teks kosong, hanya berisi spasi, NUL, dan karakter yang tidak dapat dienkode ke Shift-JIS/CP932 ditolak sebelum MES baru ditulis.
- Isi dialog dibatasi **259 byte setelah encoding**, mengikuti batas buffer renderer DC3WY. Pendekkan terjemahan jika batas ini terlampaui.
- Karakter yang tidak didukung menghasilkan error, bukan penggantian diam-diam menjadi `?`.

## CLI

Dari folder executable, gunakan contoh berikut. Ganti nama file/folder dengan milik Anda; buat folder `json` sebelum ekspor satu file.

```powershell
.\DC3WYMesTextTool_GUI.exe export "original_mes\wy_scene.mes" "json\wy_scene.json"
.\DC3WYMesTextTool_GUI.exe import "json\wy_scene.json" "original_mes\wy_scene.mes" "mes_output\wy_scene.mes" -w 46
.\DC3WYMesTextTool_GUI.exe batch-export "original_mes" "json"
.\DC3WYMesTextTool_GUI.exe batch-import "json" "original_mes" "mes_output" -w 46
```

`-w` bersifat opsional dan default-nya `46`, dengan rentang **10–100**. Jika output dihilangkan, ekspor satu file menulis JSON di sebelah input; impor CLI menulis `output_<nama MES asli>` di sebelah template. Default folder batch adalah `json_output` di folder MES input untuk ekspor, atau `mes_output` di folder MES asli untuk impor.

Untuk merapikan JSON lama menjadi format nama/dialog gabungan, gunakan MES Jepang asli sebagai acuan:

```powershell
.\DC3WYMesTextTool_GUI.exe structure-json "json\wy_scene.json" "original_mes\wy_scene.mes" "json_grouped\wy_scene.json"
.\DC3WYMesTextTool_GUI.exe batch-structure-json "json" "original_mes" "json_grouped"
```

Kedua perintah mempertahankan urutan slot teks dan menolak jumlah slot yang tidak sesuai. **Jika output tidak diberikan, JSON input diperbarui langsung.** Gunakan folder output terpisah untuk menyimpan salinan awal.

Drag-and-drop MES ke EXE mengekspor JSON di sebelahnya. Drop folder berisi MES mengekspor ke subfolder `json_output`.

## Hasil verifikasi

Build dan paket biner yang dipakai dokumentasi ini sebelumnya telah diperiksa:

- Build .NET Release berhasil tanpa warning/error; build patch dan launcher x86 Release berhasil.
- Satu skenario dengan 93 entri lulus ekspor → impor → ekspor ulang → impor ulang, termasuk pengelompokan nama, laporan layout, dan hasil impor berulang yang identik secara byte.
- Hook DLL lulus 56 kasus jarak baris dan pemeriksaan 161 nama dari JSON dengan 25 alias yang didukung, termasuk preservasi register/stack.

Pembaruan dokumentasi ini mempertahankan biner tersebut. Angka di atas berasal dari verifikasi sebelumnya; perubahan README tidak mencakup pengujian game interaktif baru.

## Build dan uji

Clone dengan submodule agar dependency Detours tersedia:

```powershell
git clone --recurse-submodules https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch.git
```

Dari folder source proyek, jalankan:

```powershell
dotnet build DC3WYMesTextTool_GUI.csproj -c Release
& ".\DC3WYPatch a\build_dc3wy_win32.bat"
```

Build GUI membutuhkan **.NET 8 SDK**. Build patch membutuhkan **Visual Studio 2022 dengan Desktop development with C++**, Windows SDK, CMake, Detours, dan berkas Locale Emulator di `DC3WYPatch a/src/le_bin`.

| Hasil | Lokasi relatif terhadap folder source |
| --- | --- |
| GUI/CLI dan paket tool | `bin/Release/net8.0-windows` |
| Berkas patch siap pakai dan RAR | `DC3WYPatch a/build/Released` |

Skrip build menyalin DLL dan launcher ke `build/Released` serta mempertahankan INI yang ada. RAR perlu dikemas ulang secara terpisah setelah build. Petunjuk verifier hook ada di [README patch](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/README.md); data audit terjemahan disediakan terpisah.

## Pemecahan masalah

- **MES asli tidak ditemukan:** pastikan nama dasarnya sama dengan JSON. Pada Single File, letakkan keduanya dalam satu folder.
- **Teks tidak muat:** periksa `.mes.layout.json`, pendekkan terjemahan atau atur pemisah baris, lalu periksa dialog dan backlog di game.
- **Aplikasi tidak terbuka:** pasang .NET 8 Desktop Runtime dan ekstrak seluruh berkas pendamping tool bersama-sama.
- **Patch tidak dimuat:** jalankan `DC3WYLauncher.exe` dari `<GAME_DIR>` dan pastikan DLL berada di sebelahnya.
- **Tidak ada output:** periksa log, izin folder output, dan pesan validasi.

Detours, Locale Emulator, dan source referensi MesTextTool memiliki lisensi masing-masing di direktorinya.
