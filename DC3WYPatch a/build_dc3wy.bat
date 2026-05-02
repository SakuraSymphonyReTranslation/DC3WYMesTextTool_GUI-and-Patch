@echo off
echo ==========================================
echo Building DC3WY Patch (Launcher + DLL)
echo   Output: build_dc3wy\
echo ==========================================

if exist build_dc3wy rmdir /s /q build_dc3wy
mkdir build_dc3wy
cd build_dc3wy

echo [1/4] Configuring CMake for DC3WY (Win32)...
cmake .. -G "Visual Studio 17 2022" -A Win32 -DBUILD_DC3WY=ON
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    cd ..
    pause
    exit /b %errorlevel%
)

echo [2/3] Compiling...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    cd ..
    pause
    exit /b %errorlevel%
)

echo [3/3] Collecting build artifacts...
copy /y ".\Release\DC3WYLauncher.exe" ".\DC3WYLauncher.exe" >nul
copy /y ".\Release\DC3WYPatch.dll"    ".\DC3WYPatch.dll"    >nul
if %errorlevel% neq 0 (
    echo [ERROR] Failed to copy compiled files.
    cd ..
    pause
    exit /b %errorlevel%
)


cd ..

echo.
echo ==========================================
echo [SUCCESS] Build completed!
echo ==========================================
echo.
echo Output files (build_dc3wy\):
echo   - DC3WYLauncher.exe     (launcher with built-in LE)
echo   - DC3WYPatch.dll        (patch DLL)
echo   - LEProc.exe            (Locale Emulator process launcher)
echo   - LocaleEmulator.dll    (LE locale hook DLL)
echo   - LoaderDll.dll         (LE bootstrapper)
echo.
echo Installation:
echo   1. Copy ALL files from build_dc3wy\ to the DC3WY game folder
echo   2. Run DC3WYLauncher.exe  (auto Japanese locale + patch)
echo.
pause
