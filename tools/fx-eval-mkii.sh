#!/usr/bin/env bash
# Evaluate an NTS-1 mkII custom FX unit: static checks + optional build.
#
# Usage:
#   ./tools/fx-eval-mkii.sh <slug> [--static-only]
#
# Exit: 0 ok | 1 static fail | 2 build skipped | 3 build fail

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SLUG="${1:-}"
STATIC_ONLY=0

shift || true
while [[ $# -gt 0 ]]; do
  case "$1" in
    --static-only) STATIC_ONLY=1; shift ;;
    *) echo "Unknown option: $1" >&2; exit 1 ;;
  esac
done

if [[ -z "${SLUG}" ]]; then
  echo "Usage: $0 <slug> [--static-only]" >&2
  exit 1
fi

UNIT_DIR="${ROOT}/src/mkii/fx/${SLUG}"
FAIL=0

log_ok() { echo "[fx-eval] OK: $*"; }
log_fail() { echo "[fx-eval] FAIL: $*" >&2; FAIL=1; }

if [[ ! -d "${UNIT_DIR}" ]]; then
  log_fail "missing unit dir ${UNIT_DIR}"
  exit 1
fi

for f in Makefile config.mk fx.h unit.cc header.c; do
  if [[ ! -f "${UNIT_DIR}/${f}" ]]; then
    log_fail "missing ${f}"
  fi
done

if ! rg -q 'PORT_COMPLETE' "${UNIT_DIR}/fx.h" 2>/dev/null; then
  log_fail "fx.h missing PORT_COMPLETE marker"
fi

if ! rg -q 'k_unit_module_(modfx|revfx|delfx)' "${UNIT_DIR}/header.c" 2>/dev/null; then
  log_fail "header.c missing k_unit_module_* target"
fi

shopt -s globstar nullglob
FILES=("${UNIT_DIR}"/*.cc "${UNIT_DIR}"/*.h "${UNIT_DIR}"/*.c)
for f in "${FILES[@]}"; do
  if rg -q '\bmalloc\s*\(|\bfree\s*\(' "${f}" 2>/dev/null; then
    log_fail "${f}: malloc/free present"
  fi
done

if [[ "${FAIL}" -ne 0 ]]; then
  exit 1
fi
log_ok "static checks for ${SLUG}"

if [[ "${STATIC_ONLY}" -eq 1 ]]; then
  exit 0
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  if [[ -d "${ROOT}/.deps/logue-sdk" ]]; then
    export LOGUE_SDK="${ROOT}/.deps/logue-sdk"
  fi
fi

if [[ -z "${LOGUE_SDK:-}" ]] || [[ ! -d "${LOGUE_SDK}" ]]; then
  echo "[fx-eval] skip build: LOGUE_SDK not set" >&2
  exit 2
fi

if "${ROOT}/tools/build-mkii-fx.sh" "${SLUG}"; then
  log_ok "build-mkii-fx.sh ${SLUG}"
  exit 0
fi

log_fail "build-mkii-fx.sh ${SLUG}"
exit 3
