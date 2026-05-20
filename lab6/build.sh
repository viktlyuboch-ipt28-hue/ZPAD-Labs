#!/usr/bin/env bash
# build.sh — налаштування та збірка CameraVision за допомогою CMake + make
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
BUILD_TYPE="${1:-Release}"   


echo "  CameraVision — Build script  ($BUILD_TYPE)"


#  створення і заходження в робочу папку
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# конфігурація CMake
echo "[1/2] Configuring with CMake..."
cmake "$SCRIPT_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

#  Білд
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
echo "Building with make ($JOBS jobs)..."
make -j"$JOBS"

echo ""
echo "Build successful!"
echo "     Binary: $BUILD_DIR/camera_vision"
echo "     Run:    ./run.sh"
