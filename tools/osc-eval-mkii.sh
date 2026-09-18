#!/usr/bin/env bash
# Evaluate an NTS-1 mkII oscillator unit: static API checks + optional build.
#
# Usage:
#   ./tools/osc-eval-mkii.sh <mkii-slug> [--static-only]
#
# Exit: 0 ok | 1 static fail | 2 build skipped (no LOGUE_SDK) | 3 build fail

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
  echo "Usage: $0 <mkii-slug> [--static-only]" >&2
  exit 1
fi

UNIT_DIR="${ROOT}/src/mkii/oscillators/${SLUG}"
FAIL=0

log_ok() { echo "[osc-eval] OK: $*"; }
log_fail() { echo "[osc-eval] FAIL: $*" >&2; FAIL=1; }

if [[ ! -d "${UNIT_DIR}" ]]; then
  log_fail "missing unit dir ${UNIT_DIR}"
  exit 1
fi

for f in Makefile config.mk osc.h unit.cc header.c; do
  if [[ ! -f "${UNIT_DIR}/${f}" ]]; then
    log_fail "missing ${f}"
  fi
done

shopt -s globstar nullglob
FILES=("${UNIT_DIR}"/*.cc "${UNIT_DIR}"/*.h "${UNIT_DIR}"/*.c)
if [[ ${#FILES[@]} -eq 0 ]]; then
  log_fail "no sources in ${UNIT_DIR}"
fi

for f in "${FILES[@]}"; do
  if rg -q 'osc_process|osc_init\s*\(|params->p\[' "${f}" 2>/dev/null; then
    log_fail "${f}: uses quarantined v1 fiction API (osc_process / params->p)"
  fi
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
  echo "[osc-eval] skip build: LOGUE_SDK not set (use --static-only or bootstrap)" >&2
  exit 2
fi

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
  echo "[osc-eval] skip build: arm-none-eabi-gcc not in PATH" >&2
  exit 2
fi

if "${ROOT}/tools/build-mkii.sh" "${SLUG}"; then
  log_ok "build-mkii.sh ${SLUG}"
  exit 0
fi

log_fail "build-mkii.sh ${SLUG}"
exit 3
