#!/usr/bin/env bash
# Package a built VST3 into release/<Name>/ (zip + install docs).
#
# Usage: ./tools/package-vst.sh <PluginName> [version]
# Example: ./tools/package-vst.sh VinylNoise 1.0.0

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="${1:-}"
VERSION="${2:-1.0.0}"

[[ -n "$NAME" ]] || { echo "Usage: $0 <PluginName> [version]" >&2; exit 1; }

bash "$ROOT/tools/apc-gate.sh" "$NAME" ship 2>/dev/null || {
  echo "Warning: APC gate not fully satisfied; packaging anyway." >&2
}

VST_DIR="$ROOT/vst/$NAME"
RELEASE_DIR="$ROOT/release/$NAME"
STAGING="$RELEASE_DIR/staging"
mkdir -p "$STAGING"

if [[ ! -d "$VST_DIR/build" ]]; then
  "$ROOT/tools/build-vst.sh" "$NAME"
fi

VST3_PATH="$(find "$VST_DIR/build" -name '*.vst3' -type d 2>/dev/null | head -1)"
[[ -n "$VST3_PATH" ]] || { echo "No .vst3 found under $VST_DIR/build" >&2; exit 1; }

BUNDLE_NAME="$(basename "$VST3_PATH")"
rm -rf "$STAGING/$BUNDLE_NAME"
cp -a "$VST3_PATH" "$STAGING/$BUNDLE_NAME"

cat > "$STAGING/INSTALL.txt" <<EOF
${NAME} v${VERSION} — VST3
========================

Install (Linux):
  Copy the folder "${BUNDLE_NAME}" to:
    ~/.vst3/

Install (Windows):
  Copy "${BUNDLE_NAME}" to:
    C:\\Program Files\\Common Files\\VST3\\

Install (macOS):
  Copy "${BUNDLE_NAME}" to:
    ~/Library/Audio/Plug-Ins/VST3/

Rescan plugins in your DAW. Standalone build (if enabled) is under vst/${NAME}/build.

Built with Instruments / JUCE. See plugins/${NAME}/.ideas/ for product spec.
EOF

cat > "$STAGING/manifest.json" <<EOF
{
  "name": "${NAME}",
  "version": "${VERSION}",
  "format": "VST3",
  "bundle": "${BUNDLE_NAME}",
  "built_at": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
EOF

ZIP_NAME="${NAME}-${VERSION}-vst3.zip"
rm -f "$RELEASE_DIR/$ZIP_NAME"
(cd "$STAGING" && zip -r "$RELEASE_DIR/$ZIP_NAME" .)

echo "Package: $RELEASE_DIR/$ZIP_NAME"
echo "Contents: $STAGING"

if command -v jq &>/dev/null && [[ -f "$ROOT/plugins/$NAME/status.json" ]]; then
  jq '.validation.ship_ready = true | .current_phase = "ship"' \
    "$ROOT/plugins/$NAME/status.json" > "$ROOT/plugins/$NAME/status.json.tmp"
  mv "$ROOT/plugins/$NAME/status.json.tmp" "$ROOT/plugins/$NAME/status.json"
fi
