#!/usr/bin/env bash
# Render MPC-oriented preview one-shots and loops from WAVE-9090 (trap) and JD Upgraded (vaporwave).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${BUILD_DIR:-$ROOT/build}"
OUT="${1:-/opt/cursor/artifacts/mpc-preview-pack}"

JD="$BUILD/OfflineRender"
WAVE="$BUILD/Wave9090OfflineRender"

if [[ ! -x "$JD" ]] || [[ ! -x "$WAVE" ]]; then
  echo "Build OfflineRender and Wave9090OfflineRender first (cmake --build build -j --target OfflineRender Wave9090OfflineRender)" >&2
  exit 1
fi

mkdir -p "$OUT/oneshots/trap-wave9090" "$OUT/oneshots/vapor-jdupgraded" "$OUT/loops/trap-wave9090" "$OUT/loops/vapor-jdupgraded"

SR=44100

echo "=== WAVE-9090 trap one-shots ==="
"$WAVE" "$OUT/oneshots/trap-wave9090/01_Slide808_C1.wav"        0 36  127 2.8 "$SR"
"$WAVE" "$OUT/oneshots/trap-wave9090/02_DarkTrapPluck_D4.wav"   2 62  110 1.4 "$SR"
"$WAVE" "$OUT/oneshots/trap-wave9090/03_MemphisCowbell_F5.wav"  4 77  120 0.9 "$SR"
"$WAVE" "$OUT/oneshots/trap-wave9090/04_DrillStab_A3.wav"       6 57  115 1.2 "$SR"
"$WAVE" "$OUT/oneshots/trap-wave9090/05_FuneralBell_C6.wav"     3 84  100 2.0 "$SR"

echo "=== WAVE-9090 trap loops (held notes, ~6.9s @ 140 BPM ≈ 4 bars) ==="
LOOP_TRAP=6.86
"$WAVE" "$OUT/loops/trap-wave9090/808_Loop_C1_140bpm.wav"     0 36  127 "$LOOP_TRAP" "$SR"
"$WAVE" "$OUT/loops/trap-wave9090/TrapPad_Loop_F3_140bpm.wav" 7 53  90  "$LOOP_TRAP" "$SR"
"$WAVE" "$OUT/loops/trap-wave9090/SmokeKeys_Loop_A3_140bpm.wav" 5 57 95 "$LOOP_TRAP" "$SR"

echo "=== JD Upgraded vaporwave one-shots ==="
"$JD" "$OUT/oneshots/vapor-jdupgraded/01_PalmMallStack_C4.wav" 64 60 100 2.5 "$SR"
"$JD" "$OUT/oneshots/vapor-jdupgraded/02_ChromeLobbyEP_C4.wav" 65 60 95  2.2 "$SR"
"$JD" "$OUT/oneshots/vapor-jdupgraded/03_SlowPacificPad_G3.wav" 66 55 90 3.0 "$SR"
"$JD" "$OUT/oneshots/vapor-jdupgraded/04_PinkAtriumBell_C5.wav" 68 72 100 2.0 "$SR"

echo "=== JD Upgraded vaporwave loops ==="
LOOP_VAPOR=8.0
"$JD" "$OUT/loops/vapor-jdupgraded/PalmMallStack_Loop_C4.wav" 64 60 85 "$LOOP_VAPOR" "$SR"
"$JD" "$OUT/loops/vapor-jdupgraded/VHSSunset_Loop_D4.wav"      67 62 80 "$LOOP_VAPOR" "$SR"
"$JD" "$OUT/loops/vapor-jdupgraded/OceanViewMall_Loop_G3.wav" 78 55 75 "$LOOP_VAPOR" "$SR"

cat > "$OUT/PRESET_CATALOG.md" <<'MD'
# MPC preview pack — preset catalog

## WAVE-9090 (trap / phonk / drill)

| Index | New name | Category | Sample in pack |
|------:|----------|----------|----------------|
| 0 | Slide 808 | TRAP | `oneshots/01_Slide808`, `loops/808_Loop` |
| 2 | Dark Trap Pluck | TRAP | `oneshots/02_DarkTrapPluck` |
| 3 | Funeral Bell | TRAP | `oneshots/05_FuneralBell` |
| 4 | Memphis Cowbell | PHONK | `oneshots/03_MemphisCowbell` |
| 5 | Smoke Keys | TRAP | `loops/SmokeKeys_Loop` |
| 6 | Drill Stab | DRILL | `oneshots/04_DrillStab` |
| 7 | Trap Pad | TRAP | `loops/TrapPad_Loop` |

Full bank: load **WAVE-9090** standalone or VST3 and browse the preset menu.

## JD Upgraded (vaporwave bank, programs 64–79)

Renamed for sample-pack / MPC marketing (programs 64–79). Curated flagship: **Palm Mall Stack (curated)** at index 64.

| Index | Name |
|------:|------|
| 64 | Palm Mall Stack (curated) |
| 65 | Chrome Lobby EP |
| 66 | Slow Pacific Pad |
| 67 | VHS Sunset Stack |
| 68 | Pink Atrium Bell |
| 69–77 | Escalator Dreams … Sundown Fax Tone (see plugin program list) |
| 78 | Ocean View Mall |
| 79 | Velvet Escalator |

Regenerate WAVs: `./scripts/render_mpc_preview_pack.sh`
MD

echo "Wrote preview pack to $OUT"
ls -la "$OUT/oneshots/trap-wave9090" "$OUT/loops/trap-wave9090" "$OUT/oneshots/vapor-jdupgraded" "$OUT/loops/vapor-jdupgraded"
