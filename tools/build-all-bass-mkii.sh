#!/usr/bin/env bash
# Scaffold (if missing) and build all mkII bass oscillators.
#
#   export LOGUE_SDK=/path/to/logue-sdk
#   ./tools/build-all-bass-mkii.sh [--scaffold-only]

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SCAFFOLD_ONLY=0
if [[ "${1:-}" == "--scaffold-only" ]]; then
  SCAFFOLD_ONLY=1
fi

python3 "${ROOT}/tools/mkii/scaffold-mkii.py" --all-bass --force

if [[ "${SCAFFOLD_ONLY}" -eq 1 ]]; then
  echo "Scaffold complete under src/mkii/oscillators/"
  exit 0
fi

if [[ -z "${LOGUE_SDK:-}" ]]; then
  echo "LOGUE_SDK not set — scaffolds written; set LOGUE_SDK and re-run to compile." >&2
  exit 0
fi

for meta in "${ROOT}"/src/mkii/oscillators/*/mkii.meta.json; do
  slug="$(basename "$(dirname "${meta}")")"
  echo "==> Building mkII ${slug}"
  "${ROOT}/tools/build-mkii.sh" "${slug}"
done

echo "All mkII bass units built."
