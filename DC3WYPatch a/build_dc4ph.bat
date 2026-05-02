@echo off
echo ==========================================
echo Building DC4PH Patch (Launcher + DLL)
echo ==========================================

if exist build_dc4ph rmdir /s /q build_dc4ph
mkdir build_dc4ph
cd build_dc4ph

echo [1/2] Configuring CMake...
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_DC4PH=ON
if %errorlevel% neq 0 (
    echo [ERROR] CMake failed.
    pause
    exit /b %errorlevel%
)

echo [2/2] Compiling...
nmake
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    pause
    exit /b %errorlevel%
)

echo.
echo ==========================================
echo [SUCCESS] Build completed!
echo ==========================================
echo.
echo Output files:
echo   - build_dc4ph\DC4PHLauncher.exe
echo   - build_dc4ph\DC4PHPatch.dll
echo.
echo Installation:
echo   1. Copy BOTH files to the DC4 Plus Harmony game folder
echo   2. Run DC4PHLauncher.exe instead of DC4PHDL.EXE
echo.
pause
