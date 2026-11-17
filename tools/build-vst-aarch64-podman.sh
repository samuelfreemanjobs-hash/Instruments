#!/usr/bin/env bash
# Cross-build vst/<Name> inside dockcross/linux-arm64 (Podman or Docker).
# Produces Contents/aarch64-linux/*.so for Melodee on MPC Sample microSD.
#
# Usage:
#   ./tools/build-vst-aarch64-podman.sh MpcMessenger
#
# Requires: podman or docker, repo with init'd vst/JUCE submodule.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${1:-}"
if [[ -z "${NAME}" ]]; then
  echo "Usage: $0 <PluginName>" >&2
  exit 1
fi

VST="${ROOT}/vst/${NAME}"
if [[ ! -f "${VST}/CMakeLists.txt" ]]; then
  echo "Not found: ${VST}/CMakeLists.txt" >&2
  exit 1
fi

RUNNER=""
if command -v podman >/dev/null 2>&1; then
  RUNNER=podman
elif command -v docker >/dev/null 2>&1; then
  RUNNER=docker
else
  echo "Install podman or docker." >&2
  exit 1
fi

IMAGE="${MPC_DOCKCROSS_IMAGE:-docker.io/dockcross/linux-arm64}"
"${RUNNER}" pull "${IMAGE}"

if [[ ! -f "${ROOT}/vst/JUCE/CMakeLists.txt" ]]; then
  echo "Init JUCE: git submodule update --init vst/JUCE" >&2
  exit 1
fi

"${RUNNER}" run --rm -v "${ROOT}:/work:z" -w "/work/vst/${NAME}" "${IMAGE}" /bin/bash -ceu "
  apt-get update -qq
  apt-get install -y -qq cmake git libasound2-dev libfreetype6-dev \\
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgtk-3-dev
  cmake -B build-aarch64 \\
    -DCMAKE_TOOLCHAIN_FILE=/dockcross/Toolchain.cmake \\
    -DCMAKE_BUILD_TYPE=Release \\
    -DMPC_VST3_HEADLESS=ON \\
    -DCMAKE_CXX_FLAGS='-march=armv8-a' \\
    -DCMAKE_C_FLAGS='-march=armv8-a' \\
    -DCOPY_PLUGIN_AFTER_BUILD=FALSE
  cmake --build build-aarch64 --config Release -j\"\$(nproc)\" --target ${NAME}_VST3
"

ART="$(find "${VST}/build-aarch64" -name '*.vst3' -type d 2>/dev/null | head -1)"
if [[ -z "${ART}" ]]; then
  echo "Build finished but no .vst3 bundle found under ${VST}/build-aarch64" >&2
  exit 1
fi

echo "VST3: ${ART}"
if [[ -x "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" ]]; then
  bash "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" "${ART}"
fi

cat <<EOF

Melodee: copy the whole $(basename "${ART}") folder to microSD Melodee/Plugins/
  K1 Cutoff · K2 Resonance · K3 Res Bass (resBassToggle > 0.5)
EOF