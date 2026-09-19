#!/usr/bin/env bash
# Generate 16× pcm_bank_XX.dlrrom (DLRROM01) for DISKLORDZ ROMPLER and run engine smoke test.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ROOT}/build"
ROM_DIR="${BUILD}/DisklordzRompler/generated"

cmake -B "${BUILD}" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12

cmake --build "${BUILD}" -j --target DisklordzRompler_GenerateRom DisklordzRomplerTests

if ! ls "${ROM_DIR}"/pcm_bank_*.dlrrom >/dev/null 2>&1; then
  echo "error: no pcm_bank_*.dlrrom in ${ROM_DIR}" >&2
  exit 1
fi

count="$(ls -1 "${ROM_DIR}"/pcm_bank_*.dlrrom | wc -l)"
echo "Factory ROM: ${count} cards in ${ROM_DIR} ($(du -sh "${ROM_DIR}" | awk '{print $1}'))"

ctest --test-dir "${BUILD}" -R DisklordzRompler --output-on-failure

echo ""
echo "Rompler loads ROM from (dev build): compile-time ${ROM_DIR}"
echo "Or set: export DISKLORDZ_ROM_DIR=${ROM_DIR}"
echo "VST3 bundle copies ROM to: .../Resources/DisklordzRom/ (build DisklordzRompler_VST3)"
