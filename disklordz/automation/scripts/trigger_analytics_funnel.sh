#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 025 \
  --prompt prompts/analytics-funnel-weekly.md \
  --subagent analytics-funnel \
  --subagent-prompt "See .cursor/agents/disklordz-analytics-funnel.md" \
  "$@"
