#!/usr/bin/env bash
# Decode microKORG .prg or .syx to text (Open-microKORG).
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
EDITOR="${REPO_ROOT}/hardware-lane/editors/open-microkorg"
CLI="${EDITOR}/cli.py"

if [[ ! -f "${CLI}" ]]; then
  echo "Init submodule: git submodule update --init hardware-lane/editors/open-microkorg" >&2
  exit 1
fi

pip install -q -r "${REPO_ROOT}/hardware-lane/requirements-editors.txt"

[[ $# -ge 1 ]] || { echo "Usage: $0 patch.prg|patch.syx" >&2; exit 1; }
python3 "${CLI}" "$@"
