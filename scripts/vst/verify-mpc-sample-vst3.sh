#!/usr/bin/env bash
# Quick check: will this VST3 bundle load on Akai MPC Sample (aarch64 Linux)?
#
# Usage:
#   ./scripts/vst/verify-mpc-sample-vst3.sh path/to/Plugin.vst3
#
# Exit 0 when every shared object under Contents/ is ELF 64-bit LSB, ARM aarch64.

set -euo pipefail

BUNDLE="${1:-}"
if [[ -z "${BUNDLE}" || ! -d "${BUNDLE}" ]]; then
  echo "Usage: $0 path/to/Plugin.vst3" >&2
  exit 1
fi

if ! command -v file >/dev/null 2>&1; then
  echo "Missing 'file' command." >&2
  exit 1
fi

SO_DIR="${BUNDLE}/Contents/aarch64-linux"
if [[ ! -d "${SO_DIR}" ]]; then
  echo "FAIL: no Contents/aarch64-linux/ in bundle (found only desktop arch?)." >&2
  echo "      Rebuild with: ./tools/build-vst-aarch64-linux.sh <PluginName>" >&2
  exit 1
fi

shopt -s nullglob
objects=("${SO_DIR}"/*)
if [[ ${#objects[@]} -eq 0 ]]; then
  echo "FAIL: aarch64-linux folder is empty." >&2
  exit 1
fi

failed=0
for obj in "${objects[@]}"; do
  [[ -f "${obj}" ]] || continue
  info="$(file -b "${obj}")"
  echo "${obj}:"
  echo "  ${info}"
  if [[ "${info}" != *"ELF 64-bit LSB"* ]] || [[ "${info}" != *"ARM aarch64"* ]]; then
    echo "  ^ expected ELF 64-bit LSB shared object, ARM aarch64" >&2
    failed=1
  fi
  if [[ "${info}" == *"x86-64"* ]] || [[ "${info}" == *"Intel 80386"* ]]; then
    echo "  ^ wrong CPU architecture for MPC Sample" >&2
    failed=1
  fi
done

if [[ "${failed}" -ne 0 ]]; then
  exit 1
fi

echo "OK: bundle looks compatible with MPC Sample (aarch64 Linux ELF)."