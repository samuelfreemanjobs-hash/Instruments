#!/usr/bin/env bash
# Regenerate tests/golden/junova/*.wav from manifest (after intentional DSP changes).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/Junova-X/JunovaOfflineRender"
MANIFEST="${ROOT}/tests/golden/junova/manifest.tsv"
OUT_DIR="${ROOT}/tests/golden/junova"

if [[ ! -x "$RENDER" ]]; then
  echo "Build JunovaOfflineRender first (cmake --build build -j --target JunovaOfflineRender)" >&2
  exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
  [[ "$line" =~ ^# ]] && continue
  [[ -z "${line// }" ]] && continue
  read -r filename program note velocity seconds sample_rate scenario <<<"$line"
  scenario="${scenario:--}"
  dest="${OUT_DIR}/${filename}"
  rm -f "$dest"
  if [[ "$scenario" != "-" ]]; then
    "$RENDER" "$dest" --scenario "$scenario" "$note" "$velocity" "$seconds" "$sample_rate"
  else
    "$RENDER" "$dest" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
  fi
  echo "Wrote $dest"
done <"$MANIFEST"

echo "Junova golden refresh complete."
