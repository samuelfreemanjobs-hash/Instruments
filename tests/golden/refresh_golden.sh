#!/usr/bin/env bash
# Regenerate golden WAVs listed in manifest.tsv (run after intentional DSP changes).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/OfflineRender"
MANIFEST="${ROOT}/tests/golden/manifest.tsv"
OUT_DIR="${ROOT}/tests/golden"

if [[ ! -x "$RENDER" ]]; then
  echo "Build OfflineRender first (cmake --build build -j)" >&2
  exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
  [[ "$line" =~ ^# ]] && continue
  [[ -z "${line// }" ]] && continue
  read -r filename program note velocity seconds sample_rate <<<"$line"
  out="${OUT_DIR}/${filename}"
  echo "Writing ${out} (program ${program}, note ${note})"
  "$RENDER" "$out" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
done <"$MANIFEST"

echo "Done. Commit updated WAVs under tests/golden/."
