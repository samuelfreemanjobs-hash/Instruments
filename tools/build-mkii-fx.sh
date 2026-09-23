#!/usr/bin/env bash
# Build one NTS-1 mkII custom FX from src/mkii/fx/<slug>.
#
# Usage:
#   export LOGUE_SDK=/path/to/logue-sdk
#   ./tools/build-mkii-fx.sh <slug>

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=tools/mkii/lib.sh
source "${ROOT}/tools/mkii/lib.sh"
if [[ -z "${LOGUE_SDK:-}" ]]; then
  if sdk="$(mkii_resolve_logue_sdk 2>/dev/null)"; then
    export LOGUE_SDK="${sdk}"
  fi
fi

SLUG="${1:-}"
if [[ -z "${SLUG}" ]]; then
  echo "Usage: $0 <fx-slug>" >&2
  exit 1
fi

UNIT_DIR="${ROOT}/src/mkii/fx/${SLUG}"
if [[ ! -d "${UNIT_DIR}" ]]; then
  echo "mkII FX unit not found: ${UNIT_DIR}" >&2
  exit 1
fi

if [[ -z "${LOGUE_SDK:-}" ]] || [[ ! -d "${LOGUE_SDK}" ]]; then
  echo "LOGUE_SDK is not set or missing" >&2
  exit 1
fi

make -C "${UNIT_DIR}" -j"$(nproc 2>/dev/null || echo 4)"
