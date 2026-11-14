#!/usr/bin/env bash
# Build a logue SDK user unit from this monorepo.
#
# Usage:
#   ./tools/build.sh <unit-path> [platform-name]
#
# Examples:
#   ./tools/build.sh oscillators/template nutekt-digital
#   ./tools/build.sh effects/modfx minilogue-xd
#
# Environment:
#   LOGUE_SDK  — path to korginc/logue-sdk checkout (required unless PLATFORMDIR is set)

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
UNIT_REL="${1:-}"
PLATFORM="${2:-nutekt-digital}"

if [[ -z "${UNIT_REL}" ]]; then
  echo "Usage: $0 <unit-path-relative-to-src> [platform]" >&2
  exit 1
fi

UNIT_DIR="${ROOT}/src/${UNIT_REL}"
if [[ ! -d "${UNIT_DIR}" ]]; then
  echo "Unit directory not found: ${UNIT_DIR}" >&2
  exit 1
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  echo "LOGUE_SDK is not set. Clone https://github.com/korginc/logue-sdk and export LOGUE_SDK." >&2
  exit 1
fi

PLAT_PATH="${LOGUE_SDK}/platform/${PLATFORM}"
if [[ ! -d "${PLAT_PATH}" ]]; then
  echo "Platform not found: ${PLAT_PATH}" >&2
  exit 1
fi

export PLATFORMDIR="${PLAT_PATH}"

# Dummy unit types use different tpl subdirs (dummy-osc, dummy-modfx, etc.).
UNIT_KIND="${UNIT_KIND:-osc}"
case "${UNIT_REL}" in
  effects/modfx) UNIT_KIND="modfx" ;;
  effects/delfx) UNIT_KIND="delfx" ;;
  effects/revfx) UNIT_KIND="revfx" ;;
esac

TPL_SRC="${PLAT_PATH}/dummy-${UNIT_KIND}/tpl"
if [[ -d "${TPL_SRC}" ]]; then
  ln -sfn "${TPL_SRC}" "${UNIT_DIR}/tpl"
elif [[ -L "${UNIT_DIR}/tpl" ]] || [[ -d "${UNIT_DIR}/tpl" ]]; then
  echo "Warning: tpl not found at ${TPL_SRC}" >&2
fi

cd "${UNIT_DIR}"
export PLATFORMDIR
export LOGUE_UNIT_KIND="${UNIT_KIND}"
make -j"$(nproc 2>/dev/null || echo 2)"
make install
