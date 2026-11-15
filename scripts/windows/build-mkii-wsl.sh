#!/usr/bin/env bash
# One-shot NTS-1 mkII build from WSL (Ubuntu). Run from repo root:
#   bash ./scripts/windows/build-mkii-wsl.sh
#
# Installs ARM gcc via apt when missing; clones logue-sdk on first run.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "${ROOT}"

need_sudo=0
for cmd in arm-none-eabi-gcc make python3 git; do
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    need_sudo=1
    break
  fi
done

if [[ "${need_sudo}" -eq 1 ]]; then
  echo "Installing build dependencies (sudo) ..."
  sudo apt-get update
  sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi make python3 git
fi

chmod +x "${ROOT}/tools/mkii-automate.sh"
"${ROOT}/tools/mkii-automate.sh" bootstrap

export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
  export GCC_BIN_PATH="$(dirname "$(command -v arm-none-eabi-gcc)")"
fi

"${ROOT}/tools/build-all-bass-mkii.sh"
"${ROOT}/tools/package-nts1.sh"

OUT="${ROOT}/release/nts1-custom-oscillators/mkII-nts1mkiiunit"
count="$(find "${OUT}" -maxdepth 1 -name '*.nts1mkiiunit' 2>/dev/null | wc -l)"
echo ""
echo "Done: ${count} mkII units in ${OUT}"
echo "Windows: from PowerShell run .\\scripts\\windows\\prepare-mkii-test.ps1"