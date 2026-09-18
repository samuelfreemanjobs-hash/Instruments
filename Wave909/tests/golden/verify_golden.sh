#!/usr/bin/env bash
set -euo pipefail

W909_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
REPO="$(cd "${W909_ROOT}/.." && pwd)"
RENDER="${REPO}/build/Wave909/Wave909OfflineRender"
DIFF="${REPO}/build/SpectralDiff"
MANIFEST="${W909_ROOT}/tests/golden/manifest.tsv"
MAX_RMS_DB="${MAX_RMS_DB:--80}"
MAX_SPECTRAL_DB="${MAX_SPECTRAL_DB:-0.25}"

if [[ ! -x "$RENDER" || ! -x "$DIFF" ]]; then
  echo "Build Wave909OfflineRender and SpectralDiff first" >&2
  exit 1
fi

while IFS= read -r line || [[ -n "$line" ]]; do
  [[ "$line" =~ ^# ]] && continue
  [[ -z "${line// }" ]] && continue
  read -r filename program note velocity seconds sample_rate <<<"$line"
  golden="${W909_ROOT}/tests/golden/${filename}"
  if [[ ! -f "$golden" ]]; then
    echo "Missing golden file: $golden" >&2
    exit 1
  fi
  tmp="$(mktemp /tmp/wave909-golden-XXXXXX.wav)"
  "$RENDER" "$tmp" "$program" "$note" "$velocity" "$seconds" "$sample_rate"
  echo "== ${filename} (program ${program}, note ${note}) =="
  "$DIFF" "$golden" "$tmp" --max-rms-db "$MAX_RMS_DB" --max-spectral-db "$MAX_SPECTRAL_DB"
  rm -f "$tmp"
done <"$MANIFEST"

echo "WAVE-909 golden comparisons passed."
