#!/usr/bin/env bash
# Style → 100 VST3 one-shots for MPC. Builds VstiOneShotRender if missing.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RENDER_BIN="${VSTI_ONE_SHOT_RENDER_BIN:-$ROOT/build/VstiOneShotRender}"

if [[ ! -x "$RENDER_BIN" ]]; then
  echo "Building VstiOneShotRender..."
  cmake -B "$ROOT/build" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER:-g++-12}" \
    -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER:-gcc-12}"
  cmake --build "$ROOT/build" -j --target VstiOneShotRender
fi

exec python3 "$ROOT/tools/vsti-one-shot/boom.py" --render-bin "$RENDER_BIN" "$@"
