#!/usr/bin/env bash
# Batch scaffold + build Play 6-pack for Akai MPC Sample (Melodee).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QUEUE="${ROOT}/disklordz/vst-factory/data/mpc_play_synth_6pack_master.json"
TEMPLATE="${ROOT}/templates/mpc-play-synth-base"
LOGS="${ROOT}/disklordz/vst-factory/logs/mpc-play6"
mkdir -p "$LOGS"

python3 -m json.tool "$QUEUE" >/dev/null

jq -c '.[]' "$QUEUE" | while read -r row; do
  name=$(echo "$row" | jq -r .name)
  code=$(echo "$row" | jq -r .code)
  dest="${ROOT}/vst/${name}"
  log="${LOGS}/${name}.log"

  echo "=== MPC Play build: $name ($code) ==="
  rm -rf "$dest"
  cp -a "$TEMPLATE" "$dest"
  sed -i "s/MpcPlayTemplate/${name}/g; s/Mptm/${code}/g; s/mpcplaytemplate/${name,,}/g" \
    "$dest/CMakeLists.template.txt" 2>/dev/null || true
  cp "$dest/CMakeLists.template.txt" "$dest/CMakeLists.txt"
  sed -i "s/MpcPlayTemplateAudioProcessor/${name}AudioProcessor/g" \
    "$dest/Source/PluginProcessor.cpp" "$dest/Source/PluginProcessor.h"

  if ! ./tools/build-vst.sh "$name" >"$log" 2>&1; then
    echo "[FAIL] x86_64 build $name — see $log"
    continue
  fi
  if ./tools/build-vst-aarch64-linux.sh "$name" >>"$log" 2>&1; then
    vst3=$(find "$dest/build-aarch64" -name "*.vst3" | head -1)
    if [[ -n "$vst3" ]]; then
      ./scripts/vst/verify-mpc-sample-vst3.sh "$vst3" >>"$log" 2>&1 && echo "[OK] aarch64 $vst3"
    fi
  else
    echo "[WARN] aarch64 cross-build skipped/failed for $name (host deps?) — see $log"
  fi
done

echo "MPC Play 6-pack batch done. Logs: $LOGS"