#!/bin/sh
# Build and run Kivo with one command on macOS. Arguments are forwarded to kivo.
set -e
cd "$(dirname "$0")"

cmake --preset macos
cmake --build build --target kivo

exec ./build/kivo "$@"
