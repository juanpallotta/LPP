#!/bin/bash
# Exit immediately if a command exits with a non-zero status
set -e

OS_TYPE="$(uname -s)"

echo "Detecting operating system..."

if [ "${OS_TYPE}" = "Darwin" ]; then
    if [ "$(id -u)" -eq 0 ]; then
        echo "Error: do not run this script as root on macOS."
        echo "Homebrew must be run as your normal user, not with sudo or root privileges."
        exit 1
    fi
    echo "Detected macOS. Installing system dependencies with Homebrew..."
    if ! command -v brew >/dev/null 2>&1; then
        echo "Error: Homebrew is not installed. Please install it first from https://brew.sh/"
        exit 1
    fi
    brew install netcdf netcdf-cxx make gcc python
else
    echo "Detected Linux. Installing system dependencies..."
    # 1. Detect the package manager and install system dependencies
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update
        sudo apt-get install -y libnetcdf-dev libnetcdff-dev libnetcdf-c++4-dev make g++ python3 python3-pip python3-venv
    elif command -v dnf >/dev/null 2>&1; then
        sudo dnf install -y netcdf-devel netcdf-cxx4-devel make gcc-c++ python3 python3-pip
    elif command -v pacman >/dev/null 2>&1; then
        sudo pacman -Sy --noconfirm netcdf netcdf-cxx make gcc python python-pip
    else
        echo "Error: Unsupported package manager. Please install netCDF, make, g++, and python3 manually."
        exit 1
    fi
fi

echo "System dependencies installed successfully."

# 2. Handle Python dependencies using a Virtual Environment
# This is crucial for newer Linux versions and macOS that block global pip installs
ENV_DIR="lidar_env"

if [ ! -d "$ENV_DIR" ]; then
    echo "Creating Python virtual environment in ./$ENV_DIR..."
    python3 -m venv "$ENV_DIR"
fi

echo "Activating virtual environment..."
source "$ENV_DIR/bin/activate"

echo "Installing Python dependencies..."
# Upgrade pip inside the virtual environment
pip install --upgrade pip

# Install all python packages in one go
pip install wget netCDF4 matplotlib scipy numpy pandas sounderpy cdsapi
