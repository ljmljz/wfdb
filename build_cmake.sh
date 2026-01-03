#!/bin/bash
# WFDB CMake Build Script for Unix-like systems

set -e

echo "Building WFDB with CMake..."
echo

# Create build directory
mkdir -p build_cmake
cd build_cmake

# Configure with CMake
echo "Configuring..."
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/install" \
  -DBUILD_SHARED_LIBS=ON \
  -DENABLE_NETFILES=ON \
  -DENABLE_FLAC=ON \
  -DBUILD_APPS=ON \
  -DBUILD_EXAMPLES=OFF

# Build
echo
echo "Building..."
cmake --build . --config Release -j$(nproc)

# Install
echo
echo "Installing..."
cmake --install . --config Release

echo
echo "Build completed successfully!"
echo "Install directory: $PWD/install"
echo

# Return to original directory
cd ..