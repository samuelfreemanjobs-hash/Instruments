#!/usr/bin/env bash
# DPCS Phase 4 verify for MeterTap — DONE = this script exits 0.
# Runs: (a) analysis ctest  (b) processor tests  (c) build-vst  (d) factory profile
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT"

export CXX="${CXX:-g++-12}"
export CC="${CC:-gcc-12}"

ANALYSIS_BUILD="${METERTAP_ANALYSIS_BUILD:-/tmp/metertap-analysis-build}"
LOG_DIR="${METERTAP_VERIFY_LOG_DIR:-/tmp/metertap-verify-logs}"
mkdir -p "$LOG_DIR"

echo "==> (a) JUCE-free analysis unit tests (V1/V2/V3/V6/drift)"
cmake -S "$ROOT/vst/factory-lib/analysis/tests" -B "$ANALYSIS_BUILD" -DCMAKE_BUILD_TYPE=Release
cmake --build "$ANALYSIS_BUILD" -j"$(nproc 2>/dev/null || echo 4)"
ctest --test-dir "$ANALYSIS_BUILD" --output-on-failure | tee "$LOG_DIR/analysis-ctest.log"

echo "==> (c) tools/build-vst.sh MeterTap (also builds processor tests)"
if [[ ! -f "$ROOT/vst/JUCE/CMakeLists.txt" ]]; then
  echo "JUCE missing under vst/JUCE — cloning pinned 7.0.12" >&2
  git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git "$ROOT/vst/JUCE"
fi
bash "$ROOT/tools/build-vst.sh" MeterTap | tee "$LOG_DIR/build-vst.log"

echo "==> (b) processor contract tests (V3/V4/V5)"
ctest --test-dir "$ROOT/vst/MeterTap/build" --output-on-failure -R MeterTapProcessor \
  | tee "$LOG_DIR/processor-ctest.log"
PROC_BIN="$ROOT/vst/MeterTap/build/MeterTapProcessorTests_artefacts/Release/MeterTapProcessorTests"
if [[ -x "$PROC_BIN" ]]; then
  "$PROC_BIN" | tee -a "$LOG_DIR/processor-ctest.log"
fi

echo "==> (d) editor render tests (theme disklordz @100%/@200%)"
ctest --test-dir "$ROOT/vst/MeterTap/build" --output-on-failure -R MeterTapEditorRender \
  | tee "$LOG_DIR/editor-render-disklordz.log"
EDITOR_BIN="$ROOT/vst/MeterTap/build/MeterTapEditorRenderTests_artefacts/Release/MeterTapEditorRenderTests"
if [[ -x "$EDITOR_BIN" ]]; then
  "$EDITOR_BIN" | tee -a "$LOG_DIR/editor-render-disklordz.log"
fi
# Copy PNGs into evidence/
EVIDENCE="$ROOT/plugins/MeterTap/evidence/editor-renders"
mkdir -p "$EVIDENCE/disklordz"
if [[ -d "$ROOT/vst/MeterTap/build/editor-renders/disklordz" ]]; then
  cp -f "$ROOT/vst/MeterTap/build/editor-renders/disklordz/"*.png "$EVIDENCE/disklordz/" || true
fi

echo "==> (e) re-theme build (THEME studio-light, no code change) + editor renders"
RETHEME_BUILD="${METERTAP_STUDIO_BUILD:-/tmp/metertap-studio-light-build}"
rm -rf "$RETHEME_BUILD"
cmake -S "$ROOT/vst/MeterTap" -B "$RETHEME_BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="${CXX}" \
  -DCMAKE_C_COMPILER="${CC}" \
  -DMETERTAP_THEME=studio-light \
  -DFACTORY_GUI_BUILD_GALLERY=OFF \
  -DFACTORY_GUI_BUILD_TESTS=OFF
cmake --build "$RETHEME_BUILD" -j"$(nproc 2>/dev/null || echo 4)" --target MeterTapEditorRenderTests
ctest --test-dir "$RETHEME_BUILD" --output-on-failure -R MeterTapEditorRender \
  | tee "$LOG_DIR/editor-render-studio-light.log"
STUDIO_BIN="$RETHEME_BUILD/MeterTapEditorRenderTests_artefacts/Release/MeterTapEditorRenderTests"
if [[ -x "$STUDIO_BIN" ]]; then
  "$STUDIO_BIN" | tee -a "$LOG_DIR/editor-render-studio-light.log"
fi
mkdir -p "$EVIDENCE/studio-light"
if [[ -d "$RETHEME_BUILD/editor-renders/studio-light" ]]; then
  cp -f "$RETHEME_BUILD/editor-renders/studio-light/"*.png "$EVIDENCE/studio-light/" || true
fi

echo "==> (f) run_business.py --profile factory --plugin MeterTap"
# Prefer local pluginval if present (offline / flaky CDN)
if [[ -x "$ROOT/vst-testing-ops/bin/pluginval" ]]; then
  export PLUGINVAL_BIN="$ROOT/vst-testing-ops/bin/pluginval"
  mkdir -p "$ROOT/build/tools/pluginval"
  cp -f "$PLUGINVAL_BIN" "$ROOT/build/tools/pluginval/pluginval"
  chmod +x "$ROOT/build/tools/pluginval/pluginval"
fi
python3 "$ROOT/vst-testing-ops/run_business.py" --profile factory --plugin MeterTap \
  | tee "$LOG_DIR/run-business.log"

# Confirm factory report ok
python3 - <<'PY' "$LOG_DIR/run-business.log"
import json, sys, re
text = open(sys.argv[1], encoding="utf-8").read()
# run_business prints a JSON object then stage lines
m = re.search(r"\{[\s\S]*\"profile\": \"factory\"[\s\S]*?\n\}", text)
if not m:
    # fallback: find first { ... } block
    start = text.find("{")
    end = text.find("\n}", start)
    if start < 0 or end < 0:
        raise SystemExit("no factory JSON in log")
    blob = text[start : end + 2]
else:
    blob = m.group(0)
report = json.loads(blob)
assert report.get("ok") is True, report
print("factory profile ok=true")
PY

python3 - <<'PY' "$ROOT/plugins/MeterTap/status.json"
import json, sys
p = sys.argv[1]
with open(p, encoding="utf-8") as f:
    s = json.load(f)
assert s.get("dpcs_phase", 0) >= 4, "status.json dpcs_phase must be >= 4"
assert s.get("validation", {}).get("code_complete") is True
assert s.get("validation", {}).get("tests_passed") is True
print("MeterTap Phase 4 status.json OK")
PY

echo "MeterTap Phase 4 VERIFY OK"
exit 0
