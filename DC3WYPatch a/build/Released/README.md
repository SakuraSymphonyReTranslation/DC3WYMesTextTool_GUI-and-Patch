# DC3WY Patch — petunjuk instalasi

**Bahasa Indonesia** | [English](Readme_en.md)

Launcher dan runtime patch untuk **Da Capo 3 With You** versi 32-bit. Naskah terjemahan dipasang terpisah.

## Instalasi

`<GAME_DIR>` berarti **folder instalasi game Anda**, yaitu folder berisi `DC3WY.EXE` dan `AdvData`. Ganti placeholder dengan lokasi sebenarnya, tanpa tanda `<` dan `>`; tidak ada drive yang diwajibkan.

1. Tutup game dan cadangkan berkas yang akan diganti, terutama executable dan `DC3WYPatch.ini`.
2. Ekstrak seluruh isi `Released_patch.rar` langsung ke `<GAME_DIR>`, sehingga `DC3WYLauncher.exe`, `DC3WYPatch.dll`, dan `DC3WYPatch.ini` berada di sebelah `DC3WY.EXE`.
3. Saat memperbarui, pertahankan INI yang sudah disesuaikan atau gabungkan pengaturan baru ke dalamnya.
4. Jalankan **`<GAME_DIR>\DC3WYLauncher.exe`** dengan working directory `<GAME_DIR>`. Untuk shortcut, isi **Start in** dengan folder itu.
5. Tekan **F11** untuk membuka **DC3WY Patch Settings**. Pilih bahasa, lalu gunakan MES Indonesia di `<GAME_DIR>\id_Data\MES` atau MES Inggris di `<GAME_DIR>\eng_data\MES`.

Launcher menyertakan Locale Emulator dan membuat `AdvData\Savegame` bila diperlukan. Gunakan launcher agar DLL patch dimuat dengan benar.

## Isi paket

- `DC3WY.EXE`: executable referensi game.
- `DC3WYLauncher.exe`: launcher patch.
- `DC3WYPatch.dll` dan `DC3WYPatch.ini`: runtime patch dan pengaturannya.
- `IconGroup103.ico` dan `scenario_titles.ini`: berkas pendukung.
- `README.md` dan `Readme_en.md`: petunjuk Indonesia dan Inggris.

## Layout dan ikon backlog

Impor terjemahan memakai **DC3WYMesTextTool_GUI** dengan default **46 kolom setengah lebar** dan rekomendasi **empat baris**. Impor menulis laporan `.mes.layout.json`; pesan panjang dilaporkan tanpa pemotongan teks. Pemisah baris manual dipertahankan.

Paket memakai `DialogueLineSpacing=-10`, `BacklogLineSpacing=-10`, `AdvancedSettings=1`, `DisableBacklogSpacing=0`, dan `ShowBacklogIcon=1`. Pitch teks normal adalah **24 px**, dengan posisi empat baris `[0, 24, 48, 72]` px. Lookup ikon mendukung 25 alias nama terjemahan. Gabungkan pengaturan ini jika memakai INI lama.

Wrap mengatur lebar saat impor; DLL mengatur jarak vertikal dan ikon saat game berjalan. Empat baris tidak dijamin untuk setiap pesan. Ruby dan teks berukuran besar mempertahankan jarak native; periksa hasilnya di dialog dan backlog.

Tool konversi membutuhkan **.NET 8 Desktop Runtime untuk Windows**. Launcher patch adalah aplikasi native yang terpisah.

## Panduan lengkap

- [Panduan tool Indonesia](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/README.md)
- [Panduan patch dan build Indonesia](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/blob/main/DC3WYPatch%20a/README.md)
- [Rilis terbaru](https://github.com/SakuraSymphonyReTranslation/DC3WYMesTextTool_GUI-and-Patch/releases/latest)

Detours dan Locale Emulator memiliki lisensi masing-masing di direktori source repositori.
