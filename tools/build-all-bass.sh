#!/usr/bin/env bash
# Build every NTS-1 multi-bass oscillator in this repo.
#
# Usage: ./tools/build-all-bass.sh [platform]
# Default platform: nutekt-digital

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PLATFORM="${1:-nutekt-digital}"

UNITS=(
  oscillators/tr808-kick
  oscillators/west-coast-moog
  oscillators/juno-rnb
)

for unit in "${UNITS[@]}"; do
  echo "==> Building ${unit} for ${PLATFORM}"
  "${ROOT}/tools/build.sh" "${unit}" "${PLATFORM}"
done

echo "All bass oscillators built."
