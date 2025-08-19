#!/usr/bin/env bash

# Name of your executable (as defined in CMakeLists.txt -> add_executable)
APP_NAME="server"

# Build directory
BUILD_DIR="build"

# Create build dir if missing
mkdir -p "$BUILD_DIR"

# Configure (only needed once, but safe to run)
cmake -S . -B "$BUILD_DIR"

# Build
cmake --build "$BUILD_DIR"

# Run if build succeeded
if [ -f "$BUILD_DIR/$APP_NAME" ]; then
    echo "🚀 Running $APP_NAME..."
    "$BUILD_DIR/$APP_NAME"
else
    echo "❌ Build failed or executable not found!"
fi
