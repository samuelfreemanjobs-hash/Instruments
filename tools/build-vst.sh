#!/usr/bin/env bash
# Build vst/<PluginName> with CMake (APC impl/test phases).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${1:-}"
[[ -n "$NAME" ]] || { echo "Usage: $0 <PluginName>" >&2; exit 1; }
VST="$ROOT/vst/$NAME"
[[ -d "$VST" ]] || { echo "Missing $VST — run /apc-impl $NAME" >&2; exit 1; }
if [[ ! -d "$ROOT/vst/JUCE" ]]; then
  echo "Init JUCE: git submodule update --init vst/JUCE" >&2
  exit 1
fi
cd "$VST"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j"$(nproc 2>/dev/null || echo 4)"
echo "Build done. Search: find build -name '*.vst3' 2>/dev/null"
