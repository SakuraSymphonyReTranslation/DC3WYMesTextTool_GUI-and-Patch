# DC3WY Patch — instalasi dan build

**Bahasa Indonesia** | [English](Readme_en.md)

Patch dan launcher ini hanya mendukung **Da Capo 3 With You** versi 32-bit. Source utama: `src/dc3wypatch.cpp`.

## Instalasi

Unduh `Released_patch.rar` dari [Releases](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest), atau gunakan paket di `build/Released`.

`<GAME_DIR>` berarti folder instalasi pilihan pemain yang berisi `DC3WY.EXE` dan `AdvData`. Ganti placeholder ini dengan lokasi game Anda, tanpa tanda `<` dan `>`; tidak ada drive yang diwajibkan.

1. Tutup game dan cadangkan berkas yang akan diganti, terutama `DC3WY.EXE` dan `DC3WYPatch.ini`.
2. Ekstrak isi paket langsung ke `<GAME_DIR>`. Pastikan launcher dan DLL patch berada di sebelah executable game.
3. Saat memperbarui, pertahankan INI yang sudah disesuaikan atau gabungkan pengaturan baru ke dalamnya.
4. Jalankan **`<GAME_DIR>\DC3WYLauncher.exe`** dengan working directory `<GAME_DIR>`; shortcut harus memakai folder itu pada **Start in**.
5. Tekan **F11** untuk membuka **DC3WY Patch Settings** dan pilih bahasa yang sesuai.

Launcher menyertakan Locale Emulator dan membuat `AdvData\Savegame` bila diperlukan. Gunakan launcher agar DLL patch dimuat dengan benar.

Paket berisi `DC3WY.EXE`, `DC3WYLauncher.exe`, `DC3WYPatch.dll`, `DC3WYPatch.ini`, `IconGroup103.ico`, `scenario_titles.ini`, serta README Indonesia dan Inggris. Naskah terjemahan dipasang terpisah ke `<GAME_DIR>\id_Data\MES` atau `<GAME_DIR>\eng_data\MES` sesuai pilihan bahasa.

## Jarak baris dan ikon backlog

- `DialogueLineSpacing=-10` dan `BacklogLineSpacing=-10` menghasilkan pitch normal **24 px**, sehingga empat baris berada pada `[0, 24, 48, 72]` px.
- Paket memakai `AdvancedSettings=1`, `DisableBacklogSpacing=0`, dan `ShowBacklogIcon=1`. Jika memakai INI lama, gabungkan pengaturan tersebut untuk menggunakan layout paket.
- Ruby dan teks berukuran besar mempertahankan jarak native dan perlu ditinjau di game.
- Lookup ikon mendukung **25 alias nama terjemahan**, termasuk bentuk underscore yang ditangani patch, lalu menggunakan lookup native game.

Word wrap dilakukan oleh importer MES, dengan default **46 kolom setengah lebar** dan rekomendasi **empat baris**. DLL mengatur jarak vertikal dan ikon. Wrap tidak menjamin setiap pesan muat dalam empat baris; teks lama yang belum diimpor ulang perlu diperiksa di dialog dan backlog. Lihat [petunjuk tool](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/README.md) untuk proses impor dan laporan `.mes.layout.json`.

## Build (Visual Studio 2022, Win32)

Pasang Visual Studio 2022 dengan **Desktop development with C++**, Windows SDK, dan CMake. Clone repositori dengan `--recurse-submodules`, atau jalankan `git submodule update --init --recursive` dari root repositori untuk mengambil Detours. Pertahankan berkas Locale Emulator di `src/le_bin`.

Jalankan `build_dc3wy_win32.bat`; skrip berpindah ke foldernya sendiri. Perintah setara dari folder `DC3WYPatch a`:

```powershell
cmake -S . -B build_dc3wy_win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build_dc3wy_win32 --config Release --target DC3WYPatch DC3WYLauncher
```

Build manual menghasilkan `build_dc3wy_win32/Release/DC3WYPatch.dll` dan `DC3WYLauncher.exe`. Skrip BAT juga menyalin keduanya ke `build/Released` dan mempertahankan INI yang ada. Gunakan konfigurasi dan berkas pendukung dari `build/Released` saat memasang build baru. RAR perlu dikemas ulang secara terpisah setelah build.

## Hook dan verifikasi

Verifikasi sebelumnya pada DLL paket lulus **56 kasus jarak baris** serta pemeriksaan **161 nama JSON dengan 25 alias yang didukung**, termasuk preservasi register/stack. Pembaruan dokumentasi ini mempertahankan biner tersebut dan tidak mencakup pengujian game interaktif baru.

Untuk menjalankan verifier dari folder `DC3WYPatch a`:

```powershell
python -X utf8 tools/verify_line_spacing.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll"
python -X utf8 tools/verify_backlog.py "<GAME_DIR>/DC3WY.EXE" "build_dc3wy_win32/Release/DC3WYPatch.dll" "<NAME_AUDIT_JSON>"
```

Ganti placeholder sebelum menjalankan perintah. Verifier membutuhkan Python dengan `pefile`, `capstone`, dan `unicorn`. `<NAME_AUDIT_JSON>` adalah file audit terjemahan dengan field `names` dan `length_mismatches`, disediakan terpisah. Pemeriksaan menjalankan hook x86 yang telah dikompilasi terhadap kode DC3WY asli untuk memeriksa jarak baris, register/stack, dan ikon nama terjemahan.

Detours dan Locale Emulator adalah dependency pihak ketiga dengan lisensi masing-masing di direktori source-nya.
