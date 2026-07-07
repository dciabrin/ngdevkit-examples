#!/bin/sh
set -e
cmake -B build-cmake -G Ninja
cmake --build build-cmake || {
    echo "=== build failed, listing generated shared assets for diagnosis ==="
    ls -la build-cmake/ngdevkit-assets || true
    exit 1
}
