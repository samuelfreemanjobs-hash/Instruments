#!/usr/bin/env bash
# Compare OfflineRender output to committed golden WAVs (see manifest.tsv).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
RENDER="${ROOT}/build/OfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
MANIFEST="${ROOT}/tests/golden/manifest.tsv"
MAX_RMS_DB="${MAX_RMS_DB:--80}"
MAX_SPECTRAL_DB="${MAX_SPECTRAL_DB:-0.25}"

if [[ ! -x "$RENDER" || ! -x "$DIFF" ]]; then
  echo "Build OfflineRender and SpectralDiff first (cmake --build build -j)" >&2
  exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
  [[ "$line" =~ ^# ]] && continue
  [[ -z "${line// }" ]] && continue
  read -r filename program note velocity seconds sample_rate <<<"$line"
  golden="${ROOT}/tests/golden/${filename}"
  if [[ ! -f "$golden" ]]; then
    echo "Missing golden file: $golden" >&2
    exit 1
  fi
  tmp="$(mktemp /tmp/jdupg-golden-XXXXXX.wav)"
  "$RENDER" "$tmp" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
  echo "== ${filename} (program ${program}, note ${note}) =="
  "$DIFF" "$golden" "$tmp" --max-rms-db "$MAX_RMS_DB" --max-spectral-db "$MAX_SPECTRAL_DB"
  rm -f "$tmp"
done <"$MANIFEST"

echo "All golden comparisons passed."
