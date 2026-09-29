#!/usr/bin/env bash
# Instruments monorepo: install Drum Loop Factory from drum-loop-factory/ subpath.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FACTORY="$ROOT/drum-loop-factory"

if [[ ! -d "$FACTORY" ]]; then
  echo "drum-loop-factory/ not found — merge PR #87 or publish from local." >&2
  exit 1
fi

export DRUM_LOOP_FACTORY_ROOT="$FACTORY"
bash "$FACTORY/scripts/cloud-agent-install.sh"
