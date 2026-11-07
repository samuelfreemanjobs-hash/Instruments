#!/usr/bin/env bash
# Fast compile loop for Cursor Terminal Agent and Cloud agents.
# On failure, writes compiler output to vst-testing-ops/error_log.txt (same path as business_pipeline).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
ERROR_LOG="$REPO_ROOT/vst-testing-ops/error_log.txt"
BUILD_DIR="$REPO_ROOT/build"
TARGET=""

usage() {
  echo "Usage: $0 [--target CMAKE_TARGET] [--no-configure]"
  echo "  Default: cmake Release build (g++-12) then cmake --build build -j"
  exit 1
}

CONFIGURE=1
while [[ $# -gt 0 ]]; do
  case "$1" in
    --target) TARGET="$2"; shift 2 ;;
    --no-configure) CONFIGURE=0; shift ;;
    -h|--help) usage ;;
    *) echo "Unknown arg: $1"; usage ;;
  esac
done

mkdir -p "$REPO_ROOT/vst-testing-ops"

if [[ "$CONFIGURE" -eq 1 ]] && [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER:-g++-12}" \
    -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER:-gcc-12}" \
    -S "$REPO_ROOT"
fi

BUILD_CMD=(cmake --build "$BUILD_DIR" -j)
if [[ -n "$TARGET" ]]; then
  BUILD_CMD+=(--target "$TARGET")
fi

LOG="$(mktemp)"
set +e
"${BUILD_CMD[@]}" 2>&1 | tee "$LOG"
STATUS=${PIPESTATUS[0]}
set -e

if [[ "$STATUS" -ne 0 ]]; then
  {
    echo "autopilot-compile.sh: build failed (exit $STATUS)"
    echo "command: ${BUILD_CMD[*]}"
    echo "--- compiler output (tail) ---"
    tail -n 120 "$LOG"
  } >"$ERROR_LOG"
  echo "Wrote $ERROR_LOG"
  exit "$STATUS"
fi

rm -f "$LOG"
echo "Build succeeded."
