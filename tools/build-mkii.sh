#!/usr/bin/env bash
# Build one NTS-1 mkII oscillator from src/mkii/oscillators/<slug>.
#
# Usage:
#   ./tools/build-mkii.sh <mkii-slug> [--scaffold-from v1/path]
#
# Examples:
#   export LOGUE_SDK=/path/to/logue-sdk
#   ./tools/build-mkii.sh tr808_kick_phonk
#   ./tools/build-mkii.sh tr808_kick_phonk --scaffold-from oscillators/tr808-kick
#
# Docker (reproducible):
#   ./tools/build-mkii.sh tr808_kick_phonk --docker

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SLUG="${1:-}"
SCAFFOLD_FROM=""
USE_DOCKER=0

shift || true
while [[ $# -gt 0 ]]; do
  case "$1" in
    --scaffold-from)
      SCAFFOLD_FROM="${2:-}"
      shift 2
      ;;
    --docker)
      USE_DOCKER=1
      shift
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
  esac
done

if [[ -z "${SLUG}" ]]; then
  echo "Usage: $0 <mkii-slug> [--scaffold-from oscillators/...] [--docker]" >&2
  exit 1
fi

UNIT_DIR="${ROOT}/src/mkii/oscillators/${SLUG}"
if [[ -n "${SCAFFOLD_FROM}" ]]; then
  python3 "${ROOT}/tools/mkii/scaffold-mkii.py" "${SCAFFOLD_FROM}" --force
fi

if [[ ! -d "${UNIT_DIR}" ]]; then
  echo "mkII unit not found: ${UNIT_DIR}" >&2
  echo "Run: python3 tools/mkii/scaffold-mkii.py --all-bass  (with LOGUE_SDK set for wasm.cc)" >&2
  exit 1
fi

if [[ "${USE_DOCKER}" -eq 1 ]]; then
  if [[ -z "${LOGUE_SDK:-}" ]]; then
    echo "LOGUE_SDK must point at a logue-sdk checkout (mounted into Docker)." >&2
    exit 1
  fi
  DOCKER_DIR="${LOGUE_SDK}/docker"
  if [[ ! -f "${DOCKER_DIR}/run_interactive.sh" ]]; then
    echo "Docker scripts not found under ${DOCKER_DIR}" >&2
    exit 1
  fi
  LINK_NAME="inst-${SLUG}"
  TARGET="${LOGUE_SDK}/platform/nts-1_mkii/${LINK_NAME}"
  ln -sfn "${UNIT_DIR}" "${TARGET}"
  echo "Linked ${TARGET} -> ${UNIT_DIR}"
  echo "Run inside logue-sdk Docker: build nts-1_mkii/${LINK_NAME}"
  exit 0
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  echo "LOGUE_SDK is not set. Clone https://github.com/korginc/logue-sdk and export LOGUE_SDK." >&2
  exit 1
fi

export LOGUE_SDK
cd "${UNIT_DIR}"
make -j"$(nproc 2>/dev/null || echo 2)"
echo "Product: ${UNIT_DIR}/*.nts1mkiiunit (or build/*.nts1mkiiunit)"
