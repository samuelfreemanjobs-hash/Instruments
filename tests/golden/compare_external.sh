#!/usr/bin/env bash
# Optional: diff tests/reference/*.wav against OfflineRender output (not used in CI).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/OfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
REF_DIR="${ROOT}/tests/reference"

if [[ ! -x "$RENDER" || ! -x "$DIFF" ]]; then
  echo "Build OfflineRender and SpectralDiff first." >&2
  exit 1
fi

if [[ ! -d "$REF_DIR" ]]; then
  echo "No tests/reference/ directory — nothing to compare."
  exit 0
fi

shopt -s nullglob
refs=("$REF_DIR"/*.wav)
if [[ ${#refs[@]} -eq 0 ]]; then
  echo "No reference WAVs in tests/reference/."
  exit 0
fi

for ref in "${refs[@]}"; do
  base="$(basename "$ref" .wav)"
  # Expected name: program{N}-note... e.g. program48-e2
  prog="${base#program}"
  prog="${prog%%-*}"
  note=60
  if [[ "$base" == *"e2"* ]]; then note=40; fi
  tmp="$(mktemp /tmp/jdupg-ext-XXXXXX.wav)"
  echo "== External: $base (program $prog, note $note) =="
  "$RENDER" "$tmp" "$prog" "$note" 100 0.5 44100
  "$DIFF" "$ref" "$tmp" --max-rms-db -20 --max-spectral-db 3
  rm -f "$tmp"
done

echo "External comparisons finished."
