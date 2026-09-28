#!/usr/bin/env bash
# Manual A/B: compare a Jun-6 V (or hardware) bounce against Junova golden scenario render.
# Usage: compare_jun6_reference.sh <reference.wav> [scenario-id] [note] [seconds]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RENDER="${ROOT}/build/Junova-X/JunovaOfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
REF="${1:?reference wav path}"
SCENARIO="${2:-ab02-fat-pad}"
NOTE="${3:-48}"
SECONDS="${4:-2.0}"

if [[ ! -f "$REF" ]]; then
  echo "Reference file not found: $REF" >&2
  exit 1
fi

tmp="$(mktemp /tmp/junova-ab-XXXXXX.wav)"
"$RENDER" "$tmp" --scenario "$SCENARIO" "$NOTE" 100 "$SECONDS" 44100
"$DIFF" "$REF" "$tmp" --max-rms-db -40 --max-spectral-db -20
rm -f "$tmp"
echo "Spectral diff complete (thresholds are loose — use ears in DAW for final call)."
