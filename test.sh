#!/bin/sh
# Build and test Kivo with one command on macOS. Arguments are forwarded to CTest.
set -e
cd "$(dirname "$0")"

cmake --preset macos -DBUILD_TESTING=ON
cmake --build build

exec ctest --test-dir build --output-on-failure "$@"
