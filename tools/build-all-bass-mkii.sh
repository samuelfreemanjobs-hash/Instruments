#!/usr/bin/env bash
# Scaffold (if missing) and build all mkII bass oscillators.
#
#   export LOGUE_SDK=/path/to/logue-sdk
#   ./tools/build-all-bass-mkii.sh [--scaffold-only]

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=tools/mkii/lib.sh
source "${ROOT}/tools/mkii/lib.sh"

SCAFFOLD_ONLY=0
BOOTSTRAP=0
for arg in "$@"; do
  case "${arg}" in
    --scaffold-only) SCAFFOLD_ONLY=1 ;;
    --bootstrap) BOOTSTRAP=1 ;;
  esac
done

if [[ "${BOOTSTRAP}" -eq 1 ]]; then
  sdk="$(mkii_ensure_logue_sdk_clone)"
  mkii_init_submodules "${sdk}"
  mkii_ensure_arm_gcc "${sdk}"
  export LOGUE_SDK="${sdk}"
elif [[ -z "${LOGUE_SDK:-}" ]]; then
  if sdk="$(mkii_resolve_logue_sdk 2>/dev/null)"; then
    export LOGUE_SDK="${sdk}"
  fi
fi

mkii_scaffold_all --force

if [[ "${SCAFFOLD_ONLY}" -eq 1 ]]; then
  echo "Scaffold complete under src/mkii/oscillators/"
  exit 0
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  echo "LOGUE_SDK not set — run: ./tools/mkii-automate.sh bootstrap" >&2
  exit 1
fi

for meta in "${ROOT}"/src/mkii/oscillators/*/mkii.meta.json; do
  slug="$(basename "$(dirname "${meta}")")"
  echo "==> Building mkII ${slug}"
  "${ROOT}/tools/build-mkii.sh" "${slug}"
done

echo "All mkII bass units built."
