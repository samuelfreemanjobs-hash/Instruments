#!/usr/bin/env bash
# Verify or merge DawDreamer + MPC-agent files from disklordz/mpc-agent after GitHub import.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
MPC="${ROOT}/disklordz/mpc-agent"
SF="${ROOT}/disklordz/sound-factory/scripts"

check() {
  local ok=0
  if [[ -d "$MPC" ]] && find "$MPC" -name '*.py' -o -name 'AGENT.md' -o -name 'SKILL.md' 2>/dev/null | grep -q .; then
    echo "OK: mpc-agent tree has content under $MPC"
  else
    echo "MISSING: push Origin handoff into $MPC (see docs/MPC_AGENT_ORIGIN_IMPORT.md)"
    ok=1
  fi
  for f in DAWDreamer_renderer.py renderers/dawdreamer_renderer.py; do
    if [[ -f "$SF/$f" ]]; then
      echo "OK: $SF/$f"
    else
      echo "MISSING: $SF/$f"
      ok=1
    fi
  done
  if rg -l 'dawdreamer|DawDreamer' "$MPC" "$SF" 2>/dev/null | head -1 >/dev/null; then
    echo "OK: DawDreamer references present"
  else
    echo "WARN: no DawDreamer string match under mpc-agent or sound-factory/scripts"
  fi
  return "$ok"
}

case "${1:---check}" in
  --check) check ;;
  *)
    echo "Usage: $0 [--check]" >&2
    exit 2
    ;;
esac
