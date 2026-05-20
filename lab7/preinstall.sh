#!/usr/bin/env bash
# preinstall.sh — Install all dependencies required to build and run CameraVision
# Я запускала на убунту, але на всякий випадок також додано встановлення для Fedora i Arch btw
set -e

echo "  CameraVision — Pre-install script"

# Перевірка дістро
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    DISTRO="unknown"
fi

echo " Detected OS: $DISTRO $VERSION_ID"

#  Debian/Ubuntu
if [[ "$DISTRO" == "ubuntu" || "$DISTRO" == "debian" ]]; then
    echo "Updating package list..."
    sudo apt-get update -y

    echo "[INFO] Installing build tools and OpenCV..."
    sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        libopencv-dev \
        libopencv-contrib-dev \
        pkg-config \
        v4l-utils \
        wget

    echo " Installing optional: GTK for OpenCV GUI (highgui)..."
    sudo apt-get install -y \
        libgtk-3-dev \
        libgstreamer1.0-dev \
        libgstreamer-plugins-base1.0-dev || true

# Fedora / RHEL / CentOS
elif [[ "$DISTRO" == "fedora" || "$DISTRO" == "rhel" || "$DISTRO" == "centos" ]]; then
    echo "Installing via dnf/yum..."
    sudo dnf install -y \
        gcc-c++ \
        cmake \
        opencv-devel \
        pkgconfig \
        wget || \
    sudo yum install -y \
        gcc-c++ cmake opencv-devel pkgconfig wget

# if I use arch btw uwu
elif [[ "$DISTRO" == "arch" || "$DISTRO" == "manjaro" ]]; then
    echo " Installing via pacman..."
    sudo pacman -Sy --noconfirm \
        base-devel cmake opencv wget

else
    echo " Unknown distro: $DISTRO"
    echo "        Please manually install: cmake, gcc/g++, libopencv-dev, wget"
fi

echo " Pre-install complete. Let's gooooo"
echo "     Run ./build.sh to compile, then ./run.sh to start."
