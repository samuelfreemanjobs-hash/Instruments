#!/usr/bin/env bash
# Step 1: Create factory ROM (16× pcm_bank_XX.dlrrom)
# Step 2: Build DISKLORDZ ROMPLER VST3 + Standalone (ROM copied into bundle)
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ROOT}/build"
ROM_DIR="${BUILD}/DisklordzRompler/generated"
ARTEFACTS="${BUILD}/DisklordzRompler/DisklordzRompler_artefacts/Release"

echo "==> Configure"
cmake -B "${BUILD}" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12

echo "==> Step 1: Create ROM (DisklordzRompler_GenerateRom)"
cmake --build "${BUILD}" -j --target DisklordzRompler_GenerateRom

count="$(ls -1 "${ROM_DIR}"/pcm_bank_*.dlrrom 2>/dev/null | wc -l)"
if [[ "${count}" -lt 16 ]]; then
  echo "error: expected 16 pcm_bank_*.dlrrom in ${ROM_DIR}, got ${count}" >&2
  exit 1
fi
echo "    ROM ready: ${count} cards, $(du -sh "${ROM_DIR}" | awk '{print $1}') at ${ROM_DIR}"

echo "==> ROM engine smoke test"
cmake --build "${BUILD}" -j --target DisklordzRomplerTests
ctest --test-dir "${BUILD}" -R DisklordzRompler --output-on-failure

echo "==> Step 2: Build VSTi (VST3 + Standalone)"
cmake --build "${BUILD}" -j --target DisklordzRompler_VST3 DisklordzRompler_Standalone

VST3="${ARTEFACTS}/VST3/DISKLORDZ ROMPLER.vst3/Contents/x86_64-linux/Resources/DisklordzRom"
STANDALONE="${ARTEFACTS}/Standalone/Resources/DisklordzRom"

for label_path in "VST3:${VST3}" "Standalone:${STANDALONE}"; do
  label="${label_path%%:*}"
  dir="${label_path#*:}"
  bundled="$(ls -1 "${dir}"/pcm_bank_*.dlrrom 2>/dev/null | wc -l || true)"
  if [[ "${bundled}" -lt 16 ]]; then
    echo "error: ${label} bundle missing ROM (found ${bundled} in ${dir})" >&2
    exit 1
  fi
  echo "    ${label}: ${bundled} ROM cards in Resources/DisklordzRom"
done

echo ""
echo "Done."
echo "  ROM source:  ${ROM_DIR}"
echo "  VST3:        ${ARTEFACTS}/VST3/DISKLORDZ ROMPLER.vst3"
echo "  Standalone:  ${ARTEFACTS}/Standalone/DISKLORDZ ROMPLER"
