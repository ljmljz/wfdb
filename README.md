# WFDB Software Package - Modernized Build System

This project modernizes the classic WFDB (WaveForm DataBase) Software Package to be compatible with contemporary development environments while preserving its core functionality.

## Overview

The WFDB Software Package provides software for working with annotated signals, originally developed by George B. Moody at MIT. This modernization effort focuses on:

- **ANSI C Compatibility**: Updated legacy ANSI C code to compile cleanly with modern GCC versions
- **Modern Build System**: Integrated CMake build system for improved cross-platform compatibility
- **Enhanced Integration**: Simplified integration with other C/C++ projects

## Key Improvements

### 1. Modernized C Code
- Updated ANSI C style code to meet current GCC compiler standards
- Resolved compilation warnings and errors from newer compiler versions
- Maintained backward compatibility while ensuring forward compatibility

### 2. CMake Build System
- Replaced traditional Makefiles with modern CMake configuration
- Cross-platform support (Windows, Linux, macOS)
- Automatic dependency detection and configuration
- Optional component building (applications, examples, XML tools)
- Configurable installation paths

### 3. Enhanced Integration
- CMake package configuration for easy integration with other projects
- Proper library linking and header management
- Optional features (network files, FLAC support, XML processing)

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C99-compatible compiler (GCC, Clang, MSVC)
- Optional: libcurl (for network file support)
- Optional: libflac (for FLAC audio support)
- Optional: expat (for XML tools)

### Basic Build
```bash
mkdir build
cd build
cmake ..
make  # or cmake --build . on Windows
```

### Windows Build
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Installation
```bash
cmake --install . --prefix /path/to/install
```

On Windows, the default installation directory is set to `dist/` in the project root.

## Configuration Options

- `BUILD_SHARED_LIBS`: Build shared libraries (default: ON)
- `ENABLE_NETFILES`: Enable network file support via libcurl (default: ON)
- `ENABLE_FLAC`: Enable FLAC audio format support (default: ON)
- `ENABLE_WAVE`: Enable WAVE format support (default: OFF)
- `BUILD_APPS`: Build command-line applications (default: ON)
- `BUILD_EXAMPLES`: Build example programs (default: ON)

## Integration with Other Projects

### Using find_package
```cmake
find_package(WFDB REQUIRED)
target_link_libraries(your_target WFDB::wfdb)
```

### Using add_subdirectory
```cmake
add_subdirectory(wfdb)
target_link_libraries(your_target wfdb)
```

## Components

### Core Library (lib/)
The main WFDB library providing signal and annotation I/O functionality.

### Applications (app/)
Command-line tools for signal processing and analysis:
- Signal readers/writers (`rdsamp`, `wrsamp`)
- Annotation tools (`rdann`, `wrann`)
- Analysis utilities (`epicmp`, `gqrs`, `wqrs`)
- Format converters and utilities

### Examples (examples/)
Sample programs demonstrating WFDB library usage.

### Optional Components
- **XML Tools (xml/)**: XML processing utilities (requires expat)
- **PSD Tools (psd/)**: Power spectral density analysis
- **Converters (convert/)**: Format conversion utilities

## Platform Notes

### Windows
- Default installation to local `dist/` folder
- Automatic library export configuration
- MSVC and MinGW compiler support

### Linux/macOS
- Standard system installation paths
- Package manager integration ready

## Original Documentation

For detailed information about the WFDB library API, file formats, and usage, please refer to the original documentation available at [PhysioNet](http://www.physionet.org/physiotools/).

## License

This software is distributed under the same licensing terms as the original WFDB package:
- WFDB library: GNU Library General Public License (LGPL)
- Applications: GNU General Public License (GPL)

See the original `COPYING` and `COPYING.LIB` files for complete license details.

## Contributing

Contributions to improve compatibility and add features are welcome. Please ensure changes maintain compatibility with the original WFDB file formats and API.

## Acknowledgments

Based on the original WFDB Software Package by George B. Moody and the PhysioNet team. This modernization effort aims to preserve the valuable functionality while making it accessible to modern development workflows.