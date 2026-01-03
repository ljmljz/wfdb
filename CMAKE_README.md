# WFDB CMake Build System

This directory contains a modern CMake build system for the WFDB Software Package, providing an alternative to the traditional configure script and Makefiles.

## Features

- **Modern CMake**: Uses contemporary CMake practices and syntax
- **Cross-platform**: Works on Windows, Linux, macOS, and other platforms
- **Configurable**: Multiple build options for different features
- **Package-friendly**: Generates pkg-config files and supports CPack
- **Parallel builds**: Takes advantage of multi-core processors
- **IDE integration**: Works with Visual Studio, CLion, and other IDEs

## Quick Start

### Windows
```batch
# Using the provided batch script
build_cmake.bat

# Or manually
mkdir build_cmake
cd build_cmake
cmake .. -G "MinGW Makefiles"
cmake --build .
cmake --install .
```

### Unix/Linux/macOS
```bash
# Using the provided shell script
chmod +x build_cmake.sh
./build_cmake.sh

# Or manually
mkdir build_cmake
cd build_cmake
cmake ..
make -j$(nproc)
make install
```

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | ON | Build shared libraries instead of static |
| `ENABLE_NETFILES` | ON | Enable network file support via libcurl |
| `ENABLE_FLAC` | ON | Enable FLAC compressed signal support |
| `ENABLE_WAVE` | OFF | Enable WAVE support (requires XView) |
| `BUILD_APPS` | ON | Build WFDB applications |
| `BUILD_EXAMPLES` | OFF | Build example programs |

## Advanced Usage

### Custom Installation Directory
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/wfdb
```

### Debug Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Disable Network Support
```bash
cmake .. -DENABLE_NETFILES=OFF
```

### Static Library Only
```bash
cmake .. -DBUILD_SHARED_LIBS=OFF
```

### Generate Package
```bash
cmake --build . --target package
```

## Directory Structure

The CMake build system maintains the same directory structure as the original build system:

```
wfdb/
├── CMakeLists.txt          # Main configuration
├── lib/CMakeLists.txt      # WFDB library
├── app/CMakeLists.txt      # WFDB applications
├── psd/CMakeLists.txt      # Power spectral density tools
├── xml/CMakeLists.txt      # XML processing tools
├── convert/CMakeLists.txt  # Format conversion tools
├── examples/CMakeLists.txt # Example programs
├── build_cmake.bat         # Windows build script
├── build_cmake.sh          # Unix build script
└── CMAKE_README.md         # This file
```

## Dependencies

### Required
- CMake 3.10 or higher
- C compiler (GCC, Clang, MSVC)

### Optional
- libcurl (for network file support)
- libflac (for FLAC compressed signal support)
- XView (for WAVE support)

## Migration from Configure Script

The CMake build system provides equivalent functionality to the configure script:

| Configure Option | CMake Option |
|------------------|--------------|
| `--prefix=PATH` | `-DCMAKE_INSTALL_PREFIX=PATH` |
| `--static` | `-DBUILD_SHARED_LIBS=OFF` |
| `--dynamic` | `-DBUILD_SHARED_LIBS=ON` |
| `--without-netfiles` | `-DENABLE_NETFILES=OFF` |
| `--without-flac` | `-DENABLE_FLAC=OFF` |
| `--with-xview` | `-DENABLE_WAVE=ON` |

## Troubleshooting

### CMake Not Found
Install CMake from https://cmake.org/download/

### Compiler Not Found
Ensure you have a C compiler installed:
- Windows: MinGW or Visual Studio
- Linux: `sudo apt-get install build-essential`
- macOS: Install Xcode Command Line Tools

### Missing Dependencies
Install development packages for optional features:
```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libflac-dev

# macOS
brew install curl flac
```

### Build Errors
1. Clean the build directory: `rm -rf build_cmake`
2. Check CMake output for missing dependencies
3. Verify compiler and toolchain installation

## Advantages over Configure Script

1. **Better IDE Integration**: Native support for modern IDEs
2. **Cross-platform**: Consistent build process across platforms
3. **Dependency Management**: Automatic detection of optional libraries
4. **Parallel Builds**: Faster compilation with multi-core support
5. **Package Generation**: Built-in support for creating installers
6. **Modern Toolchain**: Uses contemporary build tools and practices

## Contributing

When adding new source files or directories:

1. Update the appropriate `CMakeLists.txt` file
2. Follow the existing patterns for adding executables
3. Test on multiple platforms if possible
4. Update this README with any new options or requirements

## Support

For issues with the CMake build system, please report them to the WFDB project maintainers or submit a pull request with improvements.