#!/usr/bin/env bash
# Build GrimMIDI export tool and write release/GrimMIDI/midi-pack/*.mid (100 presets).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
PLUGIN_DIR="$ROOT/vst/GrimMIDI"
OUT_DIR="${1:-$ROOT/release/GrimMIDI/midi-pack}"

if [[ ! -f "$PLUGIN_DIR/../JUCE/CMakeLists.txt" ]]; then
  git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git "$ROOT/vst/JUCE"
fi

cmake -S "$PLUGIN_DIR" -B "$PLUGIN_DIR/build-export" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER="${CXX:-g++-12}" \
  -DCMAKE_C_COMPILER="${CC:-gcc-12}" \
  -DGRIMMIDI_BUILD_EXPORT_PACK=ON

cmake --build "$PLUGIN_DIR/build-export" -j"$(nproc)" --target GrimMIDIExportPack

"$PLUGIN_DIR/build-export/GrimMIDIExportPack_artefacts/Release/GrimMIDIExportPack" "--out=$OUT_DIR"
echo "MIDI pack ready: $OUT_DIR"
