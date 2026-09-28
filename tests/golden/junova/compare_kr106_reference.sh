#!/usr/bin/env bash
# Spectral diff: KR-106 render_midi vs Junova golden scenario (requires setup_kr106_reference.sh).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
KR106="${KR106_REF_DIR:-$ROOT/.reference/ultramaster_kr106}"
RENDER_KR="$KR106/tools/render-midi/render_midi"
RENDER_JX="${ROOT}/build/Junova-X/JunovaOfflineRender"
DIFF="${ROOT}/build/SpectralDiff"
SCENARIO="${1:?scenario id e.g. ab03-chorus-i}"
NOTE="${2:-60}"
SECONDS="${3:-3.0}"

for bin in "$RENDER_KR" "$RENDER_JX" "$DIFF"; do
  if [[ ! -x "$bin" ]]; then
    echo "Missing $bin — run setup_kr106_reference.sh and cmake build first." >&2
    exit 1
  fi
done

mid="$(mktemp /tmp/junova-ref-XXXXXX.mid)"
kr="$(mktemp /tmp/kr106-ref-XXXXXX.wav)"
jx="$(mktemp /tmp/junova-ref-XXXXXX.wav)"
python3 "$ROOT/Junova-X/scripts/make_test_note_mid.py" "$NOTE" "$SECONDS" "$mid"
"$RENDER_KR" "$mid" "$kr" 44100
"$RENDER_JX" "$jx" --scenario "$SCENARIO" "$NOTE" 100 "$SECONDS" 44100
echo "== KR-106 vs Junova scenario $SCENARIO note $NOTE =="
"$DIFF" "$kr" "$jx" --max-rms-db -35 --max-spectral-db -12
rm -f "$mid" "$kr" "$jx"
