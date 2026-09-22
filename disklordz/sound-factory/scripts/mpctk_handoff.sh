#!/usr/bin/env bash
# Optional MPCTK keygroup export when CLI is installed (local/Antigravity).
set -euo pipefail
INST_DIR="${1:?instrument output dir}"
if ! command -v mpctk >/dev/null 2>&1; then
  echo "mpctk not installed; see https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit"
  exit 0
fi
echo "MPCTK handoff for $INST_DIR — run mpctk with template XPJ and samples from instrument-map.json"
exit 0
