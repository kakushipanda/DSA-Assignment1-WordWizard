@echo off
setlocal enableextensions enabledelayedexpansion

echo =========================================
echo WordWizard Build Script
echo =========================================

set VCPKG_DIR=vcpkg
set BUILD_DIR=build
set TRIPLET=x64-windows

rem -----------------------------------------
rem Step 1: Ensure vcpkg exists
rem -----------------------------------------
if not exist %VCPKG_DIR% (
    echo Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git %VCPKG_DIR%
)

rem -----------------------------------------
rem Step 2: Bootstrap vcpkg
rem -----------------------------------------
if not exist %VCPKG_DIR%\vcpkg.exe (
    echo Bootstrapping vcpkg...
    call %VCPKG_DIR%\bootstrap-vcpkg.bat
)

rem -----------------------------------------
rem Step 3: Install dependencies
rem -----------------------------------------
echo Installing dependencies...
%VCPKG_DIR%\vcpkg install --triplet %TRIPLET%

if errorlevel 1 (
    echo Dependency installation failed.
    pause
    exit /b 1
)

rem -----------------------------------------
rem Step 4: Clean build directory
rem -----------------------------------------
if exist %BUILD_DIR% (
    echo Cleaning build directory...
    rmdir /s /q %BUILD_DIR%
)

mkdir %BUILD_DIR%

rem -----------------------------------------
rem Step 5: Configure CMake
rem -----------------------------------------
echo Configuring project...

cmake -B %BUILD_DIR% -S . ^
 -G "Visual Studio 17 2022" ^
 -A x64 ^
 -DCMAKE_TOOLCHAIN_FILE=./%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake

if errorlevel 1 (
    echo CMake configuration failed.
    pause
    exit /b 1
)

rem -----------------------------------------
rem Step 6: Build project
rem -----------------------------------------
echo Building project...

cmake --build %BUILD_DIR% --config Debug
cmake --build %BUILD_DIR% --config Release

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

robocopy "%BUILD_DIR%/src/Release" "%~dp0Executable" /E

echo.
echo =========================================
echo Build completed successfully.
echo =========================================
pause
