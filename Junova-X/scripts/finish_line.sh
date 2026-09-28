#!/usr/bin/env bash
# Junova-X finish-line gates — see docs/FINISH_LINE.md
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
JUNOVA="${ROOT}/Junova-X"
MODE="full"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --mode)
      MODE="${2:-full}"
      shift 2
      ;;
    *)
      echo "Usage: $0 [--mode ci|full]" >&2
      exit 2
      ;;
  esac
done

REPORT="${ROOT}/vst-testing-ops/reports/junova_finish_line.json"
REPORT_LINES="${ROOT}/vst-testing-ops/reports/.junova_finish_gates.tsv"
mkdir -p "$(dirname "$REPORT")"
: > "$REPORT_LINES"

FAILED=0

gate() {
  local id="$1" ok="$2" msg="$3"
  printf '%s\t%s\t%s\n' "$id" "$ok" "$msg" >> "$REPORT_LINES"
  if [[ "$ok" == "true" ]]; then
    echo "[PASS] $id — $msg"
  elif [[ "$ok" == "skip" ]]; then
    echo "[SKIP] $id — $msg"
  else
    echo "[FAIL] $id — $msg"
    FAILED=1
  fi
}

check_file() {
  local id="$1" path="$2"
  if [[ -f "$path" ]]; then
    gate "$id" true "$path"
  else
    gate "$id" false "missing $path"
  fi
}

# --- Tier C (always) ---
check_file C1_architecture "${JUNOVA}/ARCHITECTURE.md"
check_file C2_competitive "${JUNOVA}/docs/COMPETITIVE_JUN6.md"
check_file C2_qa_ab "${JUNOVA}/docs/QA_AB_JUN6.md"
check_file C3_finish_doc "${JUNOVA}/docs/FINISH_LINE.md"
check_file C4_gtm_readme "${JUNOVA}/gtm/README.md"
check_file C4_install_txt "${JUNOVA}/gtm/INSTALL.txt"

PRESET_CPP="${JUNOVA}/Source/Presets/FactoryPresets.cpp"
if [[ -f "$PRESET_CPP" ]]; then
  count="$(grep -c 'PresetParams {' "$PRESET_CPP" || true)"
  if [[ "$count" -ge 48 ]]; then
    gate C3_presets true "${count} factory presets"
  else
    gate C3_presets false "only ${count} presets (need 48)"
  fi
else
  gate C3_presets false "missing FactoryPresets.cpp"
fi

ART="${ROOT}/build/Junova-X/JunovaX_artefacts/Release"
if [[ -d "${ART}/VST3/Junova-X.vst3" ]]; then
  gate C5_vst3 true "VST3 bundle"
else
  gate C5_vst3 false "build JunovaX_VST3"
fi

if [[ "${JUNOVA_FINISH_SKIP_PACKAGE:-0}" != "1" ]]; then
  if bash "${JUNOVA}/gtm/package_linux.sh"; then
    zip_path="$(ls -1t "${JUNOVA}/gtm/dist/"*.zip 2>/dev/null | head -1 || true)"
    if [[ -n "$zip_path" && -f "$zip_path" ]]; then
      gate C5_package true "$(basename "$zip_path")"
    else
      gate C5_package false "package script ran but no zip in gtm/dist"
    fi
  else
    gate C5_package false "package_linux.sh failed"
  fi
else
  gate C5_package skip "JUNOVA_FINISH_SKIP_PACKAGE=1"
fi

# --- Tier B extras (full mode only) ---
if [[ "$MODE" == "full" ]]; then
  TESTS="${ROOT}/build/Junova-X/JunovaXTests"
  if [[ -x "$TESTS" ]]; then
    if "$TESTS"; then
      gate B1_junova_tests true "JunovaXTests"
    else
      gate B1_junova_tests false "JunovaXTests exited non-zero"
    fi
  else
    gate B1_junova_tests false "missing $TESTS"
  fi

  if bash "${ROOT}/tests/golden/verify_junova_golden.sh"; then
    gate B2_junova_golden true "verify_junova_golden.sh"
  else
    gate B2_junova_golden false "golden verify failed"
  fi

  KR106="${ROOT}/.reference/ultramaster_kr106/tools/render-midi/render_midi"
  if [[ -x "$KR106" ]]; then
    if bash "${ROOT}/tests/golden/junova/compare_kr106_reference.sh" ab03-chorus-i 57 3.0; then
      gate D7_kr106_ab true "KR-106 vs ab03-chorus-i"
    else
      gate D7_kr106_ab false "KR-106 spectral diff (expected until patch-aligned MIDI)"
    fi
  else
    gate D7_kr106_ab skip "KR-106 not built — run Junova-X/scripts/setup_kr106_reference.sh"
  fi
fi

python3 - "$REPORT" "$MODE" "$REPORT_LINES" <<'PY'
import json, sys
report_path, mode, lines_path = sys.argv[1], sys.argv[2], sys.argv[3]
gates = []
with open(lines_path, encoding="utf-8") as f:
    for line in f:
        line = line.rstrip("\n")
        if not line:
            continue
        parts = line.split("\t", 2)
        if len(parts) != 3:
            continue
        gid, ok, msg = parts
        gates.append({
            "id": gid,
            "ok": ok == "true",
            "skipped": ok == "skip",
            "message": msg,
        })
failed = any(not g["ok"] and not g["skipped"] for g in gates)
payload = {"product": "Junova-X", "mode": mode, "ok": not failed, "gates": gates}
with open(report_path, "w", encoding="utf-8") as f:
    json.dump(payload, f, indent=2)
print(f"Report: {report_path}")
PY

echo "Finish line mode=$MODE failed=$FAILED"
exit "$FAILED"
