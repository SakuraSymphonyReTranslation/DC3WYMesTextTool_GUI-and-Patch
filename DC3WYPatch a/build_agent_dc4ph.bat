@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

if exist build_dc4ph rmdir /s /q build_dc4ph
mkdir build_dc4ph
cd build_dc4ph

echo Configuring CMake for DC4PH...
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_DC4PH=ON
if %errorlevel% neq 0 exit /b %errorlevel%

echo Compiling DC4PH...
nmake
if %errorlevel% neq 0 exit /b %errorlevel%

echo Build SUCCESS
