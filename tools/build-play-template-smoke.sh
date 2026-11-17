#!/usr/bin/env bash
# Compile PlaySynth core template (Linux CI smoke — WebView off, no GTK).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TEMPLATE="$ROOT/templates/juce8-synth-base"
BUILD="/tmp/play-synth-template-build"
rm -rf "$BUILD"
cp -a "$TEMPLATE" "$BUILD"
cp "$BUILD/CMakeLists.template.txt" "$BUILD/CMakeLists.txt"
cmake -S "$BUILD" -B "$BUILD/build" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 \
  -DCMAKE_C_COMPILER=gcc-12 \
  -DPLAY_JUCE_DIR="$ROOT/vst/JUCE" \
  -DPLUGIN_NAME=PlaySynthBase \
  -DPLUGIN_CODE=PsBs \
  -DPLAY_SYNTH_HEADLESS_CI=ON
cmake --build "$BUILD/build" -j"$(nproc)" --target PlaySynthBase_VST3
echo "PlaySynth template VST3 smoke build OK"