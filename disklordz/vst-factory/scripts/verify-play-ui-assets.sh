#!/usr/bin/env bash
# Lightweight UI asset verification (SVG/knob contract — no pixel screenshots).
set -euo pipefail
PLUGIN_DIR="${1:?usage: verify-play-ui-assets.sh <PluginDir>}"
assets="${PLUGIN_DIR}/Assets"
fail=0
for f in index.html style.css; do
  if [[ ! -f "${assets}/${f}" ]]; then
    echo "Missing ${assets}/${f}" >&2
    fail=1
  fi
done
if [[ -f "${assets}/index.html" ]]; then
  if ! grep -qi svg "${assets}/index.html" || ! grep -qi knob "${assets}/index.html"; then
    echo "index.html should reference SVG knob markup" >&2
    fail=1
  fi
fi
if [[ "$fail" -ne 0 ]]; then
  exit 2
fi
echo "UI asset contract OK for ${PLUGIN_DIR}"