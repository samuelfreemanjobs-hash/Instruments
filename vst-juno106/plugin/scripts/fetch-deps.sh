#!/usr/bin/env bash
# Fetch VST3 SDK for Windows iPlug2 builds (not committed). Run from repo root or this script's directory.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PRODUCT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
SDK_DIR="${PRODUCT_ROOT}/plugin/Juno106/dependencies/VST3_SDK"

if [[ -d "$SDK_DIR" ]]; then
  echo "VST3_SDK already present at $SDK_DIR"
  exit 0
fi

echo "Clone Steinberg VST3 SDK into $SDK_DIR (requires git network)."
mkdir -p "$(dirname "$SDK_DIR")"
git clone --depth 1 https://github.com/steinbergmedia/vst3sdk.git "$SDK_DIR"

echo "Done. Build Juno106-vst3 on Windows per vst-juno106/REPO_HANDOFF.md"
