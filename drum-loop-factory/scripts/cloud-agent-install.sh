#!/usr/bin/env bash
set -euo pipefail

ROOT="${DRUM_LOOP_FACTORY_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
cd "$ROOT"

python3 -m pip install --upgrade pip wheel

if [[ ! -f requirements.txt ]]; then
  echo "requirements.txt not found — push Phases 1–2 from your local drum-loop-factory tree." >&2
  exit 1
fi

pip install -r requirements.txt

if [[ -f requirements-dev.txt ]]; then
  pip install -r requirements-dev.txt
fi

# Sanity check once the project tree exists.
if [[ -d tests ]]; then
  python3 -m pytest tests/ -x -q --co-only
fi
