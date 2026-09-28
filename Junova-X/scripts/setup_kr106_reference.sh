#!/usr/bin/env bash
# Clone Ultramaster KR-106 for read-only DSP study and render_midi reference WAVs (GPLv3 — no code import).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEST="${KR106_REF_DIR:-$ROOT/.reference/ultramaster_kr106}"

if [[ -d "$DEST/.git" ]]; then
  echo "KR-106 reference already at $DEST"
  exit 0
fi

mkdir -p "$(dirname "$DEST")"
git clone --depth 1 https://github.com/kayrockscreenprinting/ultramaster_kr106.git "$DEST"
echo "Cloned KR-106 to $DEST"
echo "Build: cd $DEST/tools/render-midi && make"
