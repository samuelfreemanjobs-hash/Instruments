#!/usr/bin/env bash
# Smoke-build Gearmulator JE8086TestConsole (no ROM required to compile).
# Clones upstream into gearmulator-lane/vendor/gearmulator if missing.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
VENDOR_DIR="${REPO_ROOT}/gearmulator-lane/vendor/gearmulator"
BUILD_DIR="${REPO_ROOT}/gearmulator-lane/build-je8086-console"
UPSTREAM_URL="${GEARMULATOR_REPO_URL:-https://github.com/dsp56300/gearmulator.git}"
JOBS="${GEARMULATOR_BUILD_JOBS:-$(nproc)}"

if [[ ! -d "${VENDOR_DIR}/.git" ]]; then
  mkdir -p "$(dirname "${VENDOR_DIR}")"
  git clone --depth 1 "${UPSTREAM_URL}" "${VENDOR_DIR}"
fi

git -C "${VENDOR_DIR}" submodule update --init --recursive --depth 1

mkdir -p "${BUILD_DIR}"

cmake "${VENDOR_DIR}" -B "${BUILD_DIR}" \
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
