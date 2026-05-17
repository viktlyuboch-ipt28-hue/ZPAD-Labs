#!/usr/bin/env bash
# run.sh — Запуск CameraVision
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY="$SCRIPT_DIR/build/camera_vision"
CAMERA_INDEX="${1:-0}"   # Передати індекс камери як перший аргумент (за замовчуванням 0)

echo "  CameraVision — Run script"

if [ ! -f "$BINARY" ]; then
    echo " Binary not found: $BINARY"
    echo "        Please run ./build.sh first."
    exit 1
fi

# Перевірка чи вебка взагалі є
if ! ls /dev/video"$CAMERA_INDEX" > /dev/null 2>&1; then
    echo " /dev/video$CAMERA_INDEX not found."
    echo "       Available video devices:"
    ls /dev/video* 2>/dev/null || echo "       (none found)"
    echo ""
    echo "       Trying to start anyway..."
fi

# Запуск з потрібного каталогу
cd "$SCRIPT_DIR/build"
echo " Starting with camera index $CAMERA_INDEX"
echo " Keys: 0-9/D/C/P = mode | Tab = cycle | +/- or scroll = zoom | Q/Esc = quit"
echo ""
"$BINARY" "$CAMERA_INDEX"
