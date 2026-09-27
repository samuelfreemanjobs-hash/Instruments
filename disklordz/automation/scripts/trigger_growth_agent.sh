#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
THEME="${GROWTH_LANE_THEME:-}"
if [[ -z "$THEME" ]]; then
  case "$(date -u +%u)" in
    1) THEME="DL001" ;;
    2) THEME="DL002" ;;
    3) THEME="DL004" ;;
    4) THEME="DL006" ;;
    *) THEME="MPC" ;;
  esac
fi
export THEME
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 021 \
  --theme "$THEME" \
  --prompt prompts/growth-lane-weekly.md \
  --subagent growth-lane-marketing \
  --subagent-prompt "Marketing + sound designer for lane copy and prompts. Read .cursor/agents/disklordz-growth-lane-marketing.md first." \
  "$@"
