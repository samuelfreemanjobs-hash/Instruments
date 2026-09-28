#!/usr/bin/env bash
# Compare JunovaOfflineRender output to committed golden WAVs under tests/golden/junova/
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/Junova-X/JunovaOfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
MANIFEST="${ROOT}/tests/golden/junova/manifest.tsv"
MAX_RMS_DB="${MAX_RMS_DB:--75}"
MAX_SPECTRAL_DB="${MAX_SPECTRAL_DB:-0.35}"

if [[ ! -x "$RENDER" || ! -x "$DIFF" ]]; then
  echo "Build JunovaOfflineRender and SpectralDiff first (cmake --build build -j --target JunovaOfflineRender)" >&2
  exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
  [[ "$line" =~ ^# ]] && continue
  [[ -z "${line// }" ]] && continue
  read -r filename program note velocity seconds sample_rate <<<"$line"
  golden="${ROOT}/tests/golden/junova/${filename}"
  if [[ ! -f "$golden" ]]; then
    echo "Missing golden file: $golden" >&2
    exit 1
  fi
  tmp="$(mktemp /tmp/junova-golden-XXXXXX.wav)"
  "$RENDER" "$tmp" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
  echo "== junova ${filename} (program ${program}, note ${note}) =="
  "$DIFF" "$golden" "$tmp" --max-rms-db "$MAX_RMS_DB" --max-spectral-db "$MAX_SPECTRAL_DB"
  rm -f "$tmp"
done <"$MANIFEST"

echo "All Junova golden comparisons passed."
