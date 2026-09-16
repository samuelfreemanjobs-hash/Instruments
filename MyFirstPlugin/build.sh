#!/usr/bin/env bash
# Configure + build MyFirstPlugin (terminal-only; works in Cursor, Claude Code, Antigravity, etc.)
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

if ! command -v cmake >/dev/null; then
  echo "cmake not found. Install CMake and ensure it is on PATH." >&2
  exit 1
fi

GENERATOR=()
if command -v ninja >/dev/null; then
  GENERATOR=(-G Ninja)
fi

CXX_FLAG=()
if c++ --version 2>/dev/null | grep -qi clang && ! ldconfig -p 2>/dev/null | grep -q libstdc++; then
  if command -v g++ >/dev/null; then
    CXX_FLAG=(-DCMAKE_CXX_COMPILER=g++)
  fi
fi

cmake -B build -DCMAKE_BUILD_TYPE=Release "${GENERATOR[@]}" "${CXX_FLAG[@]}"
cmake --build build -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

echo ""
echo "VST3 (Release): build/MyFirstPlugin_artefacts/Release/VST3/MyFirstPlugin.vst3"
