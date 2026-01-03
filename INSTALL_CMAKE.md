# Installing WFDB with CMake

This document provides detailed instructions for building and installing the WFDB Software Package using the modern CMake build system.

## Prerequisites

### Required Software
- **CMake**: Version 3.10 or higher
- **C Compiler**: GCC, Clang, or MSVC with C99 support

### Optional Dependencies
- **libcurl**: For network file support (NETFILES)
- **libflac**: For FLAC compressed signal support
- **XView**: For WAVE support (primarily on Unix systems)

## Installation Steps

### Step 1: Install Prerequisites

#### Windows
1. Install MinGW-w64 or Visual Studio
2. Install CMake from https://cmake.org/download/
3. (Optional) Install libcurl and libflac development packages

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libcurl4-openssl-dev libflac-dev  # optional
```

#### Linux (CentOS/RHEL/Fedora)
```bash
sudo yum install gcc cmake make
sudo yum install libcurl-devel flac-devel  # optional
```

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake
brew install curl flac  # optional
```

### Step 2: Configure Build

#### Quick Configuration
Use the provided build scripts:

**Windows:**
```batch
build_cmake.bat
```

**Unix/Linux/macOS:**
```bash
chmod +x build_cmake.sh
./build_cmake.sh
```

#### Manual Configuration
For more control over the build process:

```bash
# Create build directory
mkdir build_cmake
cd build_cmake

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/usr/local \
         -DBUILD_SHARED_LIBS=ON \
         -DENABLE_NETFILES=ON \
         -DENABLE_FLAC=ON

# Build
cmake --build . --config Release -j$(nproc)

# Install
cmake --install . --config Release
```

### Step 3: Verify Installation

After installation, verify that WFDB is working correctly:

```bash
# Check if wfdb library is installed
ls -la /usr/local/lib/*wfdb*

# Check if applications are installed
wfdb-config --version

# Test with a simple command
rdsamp -H  # Should display help
```

## Build Options

### Core Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `CMAKE_INSTALL_PREFIX` | /usr/local | Installation directory |
| `BUILD_SHARED_LIBS` | ON | Build shared libraries instead of static |

### Feature Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_NETFILES` | ON | Enable network file support via libcurl |
| `ENABLE_FLAC` | ON | Enable FLAC compressed signal support |
| `ENABLE_WAVE` | OFF | Enable WAVE support (requires XView) |
| `BUILD_APPS` | ON | Build WFDB applications |
| `BUILD_EXAMPLES` | OFF | Build example programs |

### Platform-Specific Options

#### Windows
```bash
cmake .. -G "MinGW Makefiles"  # For MinGW
cmake .. -G "Visual Studio 16 2019" -A x64  # For Visual Studio
```

#### Linux/macOS
```bash
cmake .. -DCMAKE_C_COMPILER=gcc
cmake .. -DCMAKE_C_COMPILER=clang
```

## Advanced Usage

### Static Library Build
```bash
cmake .. -DBUILD_SHARED_LIBS=OFF
```

### Debug Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Custom Installation Directory
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/wfdb
```

### Disable Optional Features
```bash
cmake .. -DENABLE_NETFILES=OFF -DENABLE_FLAC=OFF
```

### Cross-compilation
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake
```

## Creating Packages

### Source Package
```bash
cd build_cmake
cpack -G TGZ
```

### Binary Package
```bash
cd build_cmake
cpack -G DEB  # Debian package
cpack -G RPM  # RPM package
cpack -G NSIS  # Windows installer
```

## Troubleshooting

### Common Issues

#### CMake Not Found
```bash
# Check if CMake is installed
cmake --version

# Install CMake
# Windows: Download from cmake.org
# Linux: sudo apt-get install cmake
# macOS: brew install cmake
```

#### Compiler Not Found
```bash
# Check compiler
cc --version
gcc --version

# Install compiler
# Windows: Install MinGW or Visual Studio
# Linux: sudo apt-get install build-essential
# macOS: Install Xcode Command Line Tools
```

#### Missing Dependencies
```bash
# Check for libcurl
curl-config --version

# Check for libflac
pkg-config --modversion flac

# Install missing dependencies
# Linux: sudo apt-get install libcurl4-openssl-dev libflac-dev
# macOS: brew install curl flac
```

#### Build Errors
1. Clean build directory: `rm -rf build_cmake`
2. Check CMake output for specific errors
3. Verify all dependencies are installed
4. Check compiler compatibility

### Getting Help

If you encounter issues:

1. Check the [CMake README](CMAKE_README.md) for additional information
2. Review the build output for specific error messages
3. Verify your system meets all prerequisites
4. Try a minimal build first: `cmake .. -DENABLE_NETFILES=OFF -DENABLE_FLAC=OFF`

## Migration from Configure Script

If you're currently using the configure script, here's how to migrate:

| Configure Script | CMake Equivalent |
|------------------|------------------|
| `./configure` | `cmake ..` |
| `./configure --prefix=/opt/wfdb` | `cmake .. -DCMAKE_INSTALL_PREFIX=/opt/wfdb` |
| `./configure --static` | `cmake .. -DBUILD_SHARED_LIBS=OFF` |
| `./configure --without-netfiles` | `cmake .. -DENABLE_NETFILES=OFF` |
| `./configure --without-flac` | `cmake .. -DENABLE_FLAC=OFF` |
| `make` | `cmake --build .` |
| `make install` | `cmake --install .` |

## Performance Tips

1. **Parallel Builds**: Use `-j` option for faster compilation
   ```bash
   cmake --build . -- -j$(nproc)
   ```

2. **Release Builds**: Always use Release build type for production
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. **Ccache**: Use ccache for faster rebuilds
   ```bash
   cmake .. -DCMAKE_C_COMPILER_LAUNCHER=ccache
   ```

4. **Ninja**: Consider using Ninja generator for faster builds
   ```bash
   cmake .. -G Ninja
   ninja
   ```

## Next Steps

After installation:

1. Set up your WFDB environment variables
2. Test with sample data
3. Read the [WFDB Programmer's Guide](https://www.physionet.org/physiotools/wpg/)
4. Explore the example programs
5. Join the WFDB community for support and updates