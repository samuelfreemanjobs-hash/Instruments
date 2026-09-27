#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 028 \
  --prompt prompts/competitive-intel-monthly.md \
  --subagent competitive-intel \
  --subagent-prompt "See .cursor/agents/disklordz-competitive-intel.md" \
  "$@"
