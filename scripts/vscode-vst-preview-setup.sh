#!/usr/bin/env bash
# One-time/local setup for VST preview in VS Code or Cursor on the developer machine.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "== Instruments VST preview — local setup =="

if ! command -v python3 >/dev/null; then
  echo "ERROR: python3 required" >&2
  exit 1
fi

if ! command -v cmake >/dev/null; then
  echo "ERROR: cmake not found. Install CMake 3.22+ and re-run." >&2
  exit 1
fi

case "$(uname -s)" in
  Linux)
    if command -v apt-get >/dev/null; then
      echo "Tip: sudo apt-get install -y build-essential g++-12 cmake sox libasound2-dev \\"
      echo "  libfreetype6-dev libfontconfig1-dev libgl1-mesa-dev libx11-dev libxrandr-dev \\"
      echo "  libxcursor-dev libxinerama-dev libxext-dev libcurl4-openssl-dev"
    fi
    CXX="${CMAKE_CXX_COMPILER:-g++-12}"
    CC="${CMAKE_C_COMPILER:-gcc-12}"
    ;;
  Darwin)
    CXX="${CMAKE_CXX_COMPILER:-clang++}"
    CC="${CMAKE_C_COMPILER:-clang}"
    if ! command -v sox >/dev/null; then
      echo "Tip: brew install sox  # optional peak metering in preview manifest"
    fi
    ;;
  *)
    CXX="${CMAKE_CXX_COMPILER:-c++}"
    CC="${CMAKE_C_COMPILER:-cc}"
    ;;
esac

if [[ ! -f build/CMakeCache.txt ]]; then
  echo "Configuring CMake (Release)…"
  cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC"
else
  echo "CMake cache already present."
fi

echo "Building preview targets (OfflineRender + Standalone smoke)…"
cmake --build build -j --target OfflineRender JDUpgraded_Standalone Wave909Tests

echo ""
echo "Setup OK. Next:"
echo "  python3 vst-testing-ops/preview_agent.py"
echo "  or VS Code: Run Task → Instruments: VST preview WAVs"
echo "  WAV output: vst-testing-ops/previews/latest/"
