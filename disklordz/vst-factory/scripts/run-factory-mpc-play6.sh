#!/usr/bin/env bash
# MPC Play 6-pack — scaffold, build, optional Claude Code Layer A/B pass (Melodee / aarch64).
#   ./disklordz/vst-factory/scripts/run-factory-mpc-play6.sh --skip-claude --only MpcPlayPhonk
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib-factory-claude.sh
source "${SCRIPT_DIR}/lib-factory-claude.sh"

ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
QUEUE="${ROOT}/disklordz/vst-factory/data/mpc_play_synth_6pack_master.json"
TEMPLATE="${ROOT}/templates/mpc-play-synth-base"
CONTRACT="${ROOT}/disklordz/vst-factory/data/mpc_play_factory_contract.md"
LOGS="${ROOT}/disklordz/vst-factory/logs/mpc-play6"
REPORT="${LOGS}/morning_report.csv"

SKIP_CLAUDE=0
MAX_TURNS=24
ONLY=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-claude) SKIP_CLAUDE=1; shift ;;
    --max-turns) MAX_TURNS="$2"; shift 2 ;;
    --only) ONLY="$2"; shift 2 ;;
    -h|--help)
      echo "Usage: $0 [--skip-claude] [--max-turns N] [--only MpcPlayPhonk]"
      exit 0
      ;;
    *) echo "Unknown arg: $1" >&2; exit 2 ;;
  esac
done

mkdir -p "$LOGS"
python3 -m json.tool "$QUEUE" >/dev/null
contract_body="$(cat "$CONTRACT")"
echo "Plugin,Code,Status,Aarch64,Minutes" >"$REPORT"

jq -c '.[]' "$QUEUE" | while read -r row; do
  name=$(echo "$row" | jq -r .name)
  code=$(echo "$row" | jq -r .code)
  layer_a=$(echo "$row" | jq -r '.layerA')
  layer_b=$(echo "$row" | jq -r '.layerB')
  k123=$(echo "$row" | jq -r '.k1_k2_k3')
  presets=$(echo "$row" | jq -r '.presets')
  voices=$(echo "$row" | jq -r '.voices // 8')

  if [[ -n "$ONLY" && "$name" != "$ONLY" ]]; then
    continue
  fi

  dest="${ROOT}/vst/${name}"
  log="${LOGS}/${name}.log"
  status="FAILED_BUILD"
  aarch64_status="SKIPPED"
  start_ts=$(date +%s)

  echo "=== MPC Play: $name ($code) ==="
  rm -rf "$dest"
  cp -a "$TEMPLATE" "$dest"
  sed -i "s/MpcPlayTemplate/${name}/g; s/Mptm/${code}/g" \
    "${dest}/CMakeLists.template.txt"
  cp "${dest}/CMakeLists.template.txt" "${dest}/CMakeLists.txt"
  sed -i "s/MpcPlayTemplateAudioProcessor/${name}AudioProcessor/g" \
    "${dest}/Source/PluginProcessor.cpp" "${dest}/Source/PluginProcessor.h"
  sed -i "s/class MpcPlayTemplateAudioProcessor/class ${name}AudioProcessor/g" \
    "${dest}/Source/PluginProcessor.h"

  : >"$log"

  if [[ "$SKIP_CLAUDE" -eq 0 ]]; then
    customize_prompt="$(cat <<EOF
Read AGENTS.md, docs/CLAUDE_VST_ENGINEER.md, and docs/MPC_SAMPLE_VST3.md (repo: ${ROOT}).

Headless MPC Sample plugin '${name}' (code ${code}, max voices ${voices}).
Customize ONLY before first full build:
- templates/juce8-synth-base/Source/LayerAEngine.h patterns copied into this tree OR edit shared headers if you extend include path
- Source/FactoryPrograms.h — 8 programs: ${presets}
- Source/MacroRouter.h — map K1/K2/K3 first: ${k123}
- mpc_play::kVoiceCount in FactoryPrograms.h if not ${voices}

Do NOT enable WebView. hasEditor() stays false.
LAYER A: ${layer_a}
LAYER B: ${layer_b}

${contract_body}

Then from repo root: ./tools/build-vst.sh ${name} && ./tools/build-vst-aarch64-linux.sh ${name}
EOF
)"
    pushd "$ROOT" >/dev/null
    factory_run_claude "$customize_prompt" "$log" "$MAX_TURNS" || true
    popd >/dev/null
  fi

  if "${ROOT}/tools/build-vst.sh" "$name" >>"$log" 2>&1; then
    status="COMPILED_X86"
    if "${ROOT}/tools/build-vst-aarch64-linux.sh" "$name" >>"$log" 2>&1; then
      vst3=$(find "${dest}/build-aarch64" -name "*.vst3" 2>/dev/null | head -1)
      if [[ -n "$vst3" ]] && "${ROOT}/scripts/vst/verify-mpc-sample-vst3.sh" "$vst3" >>"$log" 2>&1; then
        aarch64_status="VERIFIED"
        status="PASSED_MPC_VERIFY"
      else
        aarch64_status="BUILD_ONLY"
        status="COMPILED_AARCH64"
      fi
    fi
  fi

  end_ts=$(date +%s)
  minutes=$(python3 -c "print(round((${end_ts} - ${start_ts}) / 60.0, 1))")
  echo "${name},${code},${status},${aarch64_status},${minutes}" >>"$REPORT"
  echo "=> $name $status aarch64=${aarch64_status} (${minutes} min)"
done

echo ""
echo "MPC Play 6-pack batch done."
column -t -s, "$REPORT" 2>/dev/null || cat "$REPORT"
echo "Logs: $LOGS"