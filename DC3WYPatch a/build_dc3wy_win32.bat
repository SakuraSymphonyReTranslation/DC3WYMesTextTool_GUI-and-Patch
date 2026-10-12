@echo off
setlocal
cd /d "%~dp0"
echo Building Da Capo 3 With You patch and launcher...
cmake -S . -B build_dc3wy_win32 -G "Visual Studio 17 2022" -A Win32
if errorlevel 1 exit /b 1
cmake --build build_dc3wy_win32 --config Release --target DC3WYPatch DC3WYLauncher
if errorlevel 1 exit /b 1
if not exist "build\Released" mkdir "build\Released"
for %%F in (DC3WYPatch.dll DC3WYLauncher.exe) do (
    copy /y "build_dc3wy_win32\Release\%%F" "build_dc3wy_win32\%%F" >nul
    if errorlevel 1 exit /b 1
    copy /y "build_dc3wy_win32\Release\%%F" "build\%%F" >nul
    if errorlevel 1 exit /b 1
    copy /y "build_dc3wy_win32\Release\%%F" "build\Released\%%F" >nul
    if errorlevel 1 exit /b 1
)
echo DC3WY build ready in build\Released. Existing INI settings are preserved.
exit /b 0
