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
HOLD_SEC="${3:-3.0}"

for bin in "$RENDER_KR" "$RENDER_JX" "$DIFF"; do
  if [[ ! -x "$bin" ]]; then
    echo "Missing $bin — run setup_kr106_reference.sh and cmake build first." >&2
    exit 1
  fi
done

mid="$(mktemp /tmp/junova-ref-XXXXXX.mid)"
kr="$(mktemp /tmp/kr106-ref-XXXXXX.wav)"
jx="$(mktemp /tmp/junova-ref-XXXXXX.wav)"
if [[ -f "$ROOT/Junova-X/fixtures/kr106_scenarios.json" ]]; then
  python3 "$ROOT/Junova-X/scripts/make_kr106_golden_mid.py" "$SCENARIO" "$mid"
else
  python3 "$ROOT/Junova-X/scripts/make_test_note_mid.py" "$NOTE" "$HOLD_SEC" "$mid"
fi
# Duration/note for Junova from fixture when present
if [[ -f "$ROOT/Junova-X/fixtures/kr106_scenarios.json" ]]; then
  read -r FIX_NOTE FIX_SEC < <(
    python3 "$ROOT/Junova-X/scripts/kr106_fixture_duration.py" "$SCENARIO"
  )
  NOTE="${FIX_NOTE:-$NOTE}"
  HOLD_SEC="${FIX_SEC:-$HOLD_SEC}"
fi
"$RENDER_KR" "$mid" "$kr" 44100
"$RENDER_JX" "$jx" --scenario "$SCENARIO" "$NOTE" 100 "$HOLD_SEC" 44100
echo "== KR-106 vs Junova scenario $SCENARIO note $NOTE =="
# Smoke thresholds after patch-aligned MIDI (tight timbre match = future DSP WO).
MAX_RMS_DB="${KR106_MAX_RMS_DB:--5}"
MAX_SPECTRAL_DB="${KR106_MAX_SPECTRAL_DB:-25}"
"$DIFF" "$kr" "$jx" --max-rms-db "$MAX_RMS_DB" --max-spectral-db "$MAX_SPECTRAL_DB"
rm -f "$mid" "$kr" "$jx"
