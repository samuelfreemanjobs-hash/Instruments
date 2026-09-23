#!/usr/bin/env bash
# Smoke-build Gearmulator JE8086TestConsole (no ROM required to compile).
# Uses git submodule at gearmulator-lane/gearmulator.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GEARMULATOR_DIR="${REPO_ROOT}/gearmulator-lane/gearmulator"
BUILD_DIR="${REPO_ROOT}/gearmulator-lane/build-je8086-console"
JOBS="${GEARMULATOR_BUILD_JOBS:-$(nproc)}"

if [[ ! -f "${GEARMULATOR_DIR}/CMakeLists.txt" ]]; then
  echo "Missing ${GEARMULATOR_DIR}. Run from repo root:" >&2
  echo "  git submodule update --init --recursive gearmulator-lane/gearmulator" >&2
  exit 1
fi

git -C "${REPO_ROOT}" submodule update --init --recursive gearmulator-lane/gearmulator

mkdir -p "${BUILD_DIR}"

cmake "${GEARMULATOR_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER:-g++-12}" \
  -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER:-gcc-12}" \
  -Dgearmulator_BUILD_JUCEPLUGIN=OFF \
  -Dgearmulator_SYNTH_OSIRUS=OFF \
  -Dgearmulator_SYNTH_OSTIRUS=OFF \
  -Dgearmulator_SYNTH_VAVRA=OFF \
  -Dgearmulator_SYNTH_XENIA=OFF \
  -Dgearmulator_SYNTH_NODALRED2X=OFF \
  -Dgearmulator_SYNTH_JE8086=ON \
  -Dgearmulator_SYNTH_88EMU=OFF

cmake --build "${BUILD_DIR}" --target JE8086TestConsole -j"${JOBS}"

echo "Built: ${BUILD_DIR}/source/ronaldo/je8086/jeTestConsole/JE8086TestConsole"
