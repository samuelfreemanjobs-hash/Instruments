#!/usr/bin/env bash
# PlaySynth 6-pack factory (Linux / bash + optional Claude Code)
#   ./disklordz/vst-factory/scripts/run-factory-play6.sh
#   ./disklordz/vst-factory/scripts/run-factory-play6.sh --skip-claude --only PhonkCowbell94
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib-factory-claude.sh
source "${SCRIPT_DIR}/lib-factory-claude.sh"

ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
QUEUE="${ROOT}/disklordz/vst-factory/data/play_synth_6pack_master.json"
TEMPLATE="${ROOT}/templates/juce8-synth-base"
CONTRACT="${ROOT}/disklordz/vst-factory/data/play_synth_factory_contract.md"
UI_VERIFY="${SCRIPT_DIR}/verify-play-ui-assets.sh"
LOGS="${ROOT}/disklordz/vst-factory/logs/play6"
OUT="${ROOT}/disklordz/vst-factory/builds_ready"
REPORT="${LOGS}/morning_report.csv"

SKIP_CLAUDE=0
MAX_TURNS=28
HEADLESS_CI=1
ONLY=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-claude) SKIP_CLAUDE=1; shift ;;
    --max-turns) MAX_TURNS="$2"; shift 2 ;;
    --with-webview) HEADLESS_CI=0; shift ;;
    --only) ONLY="$2"; shift 2 ;;
    -h|--help)
      echo "Usage: $0 [--skip-claude] [--max-turns N] [--with-webview] [--only PluginName]"
      exit 0
      ;;
    *) echo "Unknown arg: $1" >&2; exit 2 ;;
  esac
done

mkdir -p "$LOGS" "$OUT"
python3 -m json.tool "$QUEUE" >/dev/null
contract_body="$(cat "$CONTRACT")"

echo "Plugin,Code,Status,AudioSmoke,UiContract,Minutes" >"$REPORT"

jq -c '.[]' "$QUEUE" | while read -r row; do
  name=$(echo "$row" | jq -r .name)
  code=$(echo "$row" | jq -r .code)
  build_root=$(echo "$row" | jq -r '.build_root // .name')
  theme=$(echo "$row" | jq -r '.theme // ""')
  layer_a=$(echo "$row" | jq -r '.layerA')
  layer_b=$(echo "$row" | jq -r '.layerB')
  macros=$(echo "$row" | jq -r '.macros // ""')
  presets=$(echo "$row" | jq -r '.presets')

  if [[ -n "$ONLY" && "$name" != "$ONLY" ]]; then
    continue
  fi

  plugin_dir="${ROOT}/${build_root}"
  log="${LOGS}/${name}.log"
  preview_wav="${LOGS}/${name}_preview.wav"
  status="FAILED_BUILD"
  audio_status="SKIPPED"
  ui_status="SKIPPED"
  start_ts=$(date +%s)

  echo ""
  echo "=========================================="
  echo "BUILDING: $name ($code)"
  echo "=========================================="

  rm -rf "$plugin_dir"
  cp -a "$TEMPLATE" "$plugin_dir"
  cp "${plugin_dir}/CMakeLists.template.txt" "${plugin_dir}/CMakeLists.txt"

  : >"$log"
  if bash "$UI_VERIFY" "$plugin_dir" >>"$log" 2>&1; then
    ui_status="UI_ASSETS_OK"
  fi

  pushd "$plugin_dir" >/dev/null
  cmake_args=(
    -S . -B build
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_CXX_COMPILER=g++-12
    -DCMAKE_C_COMPILER=gcc-12
    -DPLAY_JUCE_DIR="${ROOT}/vst/JUCE"
    -DPLUGIN_NAME="$name"
    -DPLUGIN_CODE="$code"
  )
  if [[ "$HEADLESS_CI" -eq 1 ]]; then
    cmake_args+=(-DPLAY_SYNTH_HEADLESS_CI=ON)
  fi

  echo "CMake configure..." | tee -a "$log"
  if cmake "${cmake_args[@]}" >>"$log" 2>&1 && cmake --build build -j"$(nproc)" --config Release >>"$log" 2>&1; then
  vst3=$(find build -name "*.vst3" -type d 2>/dev/null | head -1)
  standalone=$(find build -path "*Standalone/${name}" -type f 2>/dev/null | head -1)
  [[ -n "$vst3" ]] && status="COMPILED"

  pluginval=""
  if command -v pluginval >/dev/null 2>&1; then
    pluginval=pluginval
  elif [[ -x "${ROOT}/tools/pluginval" ]]; then
    pluginval="${ROOT}/tools/pluginval"
  fi
  if [[ -n "$vst3" && -n "$pluginval" ]]; then
    if "$pluginval" --strictness-level 5 --validate-in-process --validate "$vst3" >>"$log" 2>&1; then
      status="PASSED_PLUGINVAL_5"
    else
      status="FAILED_PLUGINVAL"
    fi
  fi

  if [[ -n "$standalone" && -x "$standalone" ]]; then
    rm -f "$preview_wav" "${TMPDIR:-/tmp}/play_render_preview.wav"
    if "$standalone" --render-test >>"$log" 2>&1; then
      audio_status="RENDER_OK"
    else
      audio_status="RENDER_FAIL"
    fi
    temp_wav="${TMPDIR:-/tmp}/play_render_preview.wav"
    if [[ -f "$temp_wav" ]]; then
      cp "$temp_wav" "$preview_wav"
      audio_status="RENDER_OK"
    fi
  fi

  if [[ "$SKIP_CLAUDE" -eq 0 ]] && [[ "$status" =~ COMPILED|PASSED ]]; then
    customize_prompt="$(cat <<EOF
Read AGENTS.md and docs/CLAUDE_VST_ENGINEER.md (repo root: ${ROOT}).

The PlaySynth **core** already compiles (APVTS, voices, arp/scale, render smoke).
Only customize these files for '${name}' (PLUGIN_CODE ${code}):
- Source/LayerAEngine.h (Layer A DSP)
- Source/LayerBEngine.h (Layer B DSP)
- Source/MacroRouter.h (macro curves if needed)
- Assets/index.html, Assets/style.css, Assets/app.js (theme: ${theme})
- Source/FactoryPresets.h names + 12 blend/macro rows matching: ${presets}

Do NOT rewrite PluginProcessor editor/CMake unless build breaks.
THEME: ${theme}
LAYER A: ${layer_a}
LAYER B: ${layer_b}
MACROS: ${macros}

${contract_body}

Rebuild Release (cmake --build build -j) and ensure pluginval + --render-test still pass when available.
EOF
)"
    factory_run_claude "$customize_prompt" "$log" "$MAX_TURNS" || true
    cmake --build build -j"$(nproc)" --config Release >>"$log" 2>&1 || true
    vst3=$(find build -name "*.vst3" -type d 2>/dev/null | head -1)
    if [[ -n "$vst3" && -n "$pluginval" ]]; then
      if "$pluginval" --strictness-level 5 --validate-in-process --validate "$vst3" >>"$log" 2>&1; then
        status="PASSED_LEVEL_5_CUSTOMIZED"
      fi
    fi
  fi

  if [[ -n "$vst3" && "$status" =~ PASSED|COMPILED ]]; then
    cp -a "$vst3" "$OUT/"
  fi
  else
    echo "CMake/build failed — see $log" >&2
  fi
  popd >/dev/null

  end_ts=$(date +%s)
  minutes=$(python3 -c "print(round((${end_ts} - ${start_ts}) / 60.0, 1))")
  echo "${name},${code},${status},${audio_status},${ui_status},${minutes}" >>"$REPORT"
  echo "=> $name $status (${minutes} min)"
done

echo ""
echo "================ MORNING FACTORY REPORT ================"
column -t -s, "$REPORT" 2>/dev/null || cat "$REPORT"
echo "CSV: $REPORT"