#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/Resources/Cleanroom/jdupg_cleanroom.rom"
mkdir -p "${ROOT}/Resources/Cleanroom"
cmake -B "${ROOT}/build" -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build "${ROOT}/build" --target GenerateCleanroomRom -j
"${ROOT}/build/GenerateCleanroomRom" "${OUT}"
echo "Wrote ${OUT}"
