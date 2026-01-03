@echo off
REM WFDB CMake Build Script for Windows

echo Building WFDB with CMake...
echo.

REM Create build directory
if not exist build_cmake mkdir build_cmake
cd build_cmake

REM Configure with CMake
echo Configuring...
cmake .. -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX="%CD%\install" ^
  -DBUILD_SHARED_LIBS=ON ^
  -DENABLE_NETFILES=ON ^
  -DENABLE_FLAC=ON ^
  -DBUILD_APPS=ON ^
  -DBUILD_EXAMPLES=OFF

if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

REM Install
echo.
echo Installing...
cmake --install . --config Release

echo.
echo Build completed successfully!
echo Install directory: %CD%\install
echo.

REM Return to original directory
cd ..

pause