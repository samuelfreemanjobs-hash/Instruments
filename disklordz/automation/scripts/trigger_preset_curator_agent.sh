#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$DIR/../../.." && pwd)"
node "$ROOT/disklordz/automation/scripts/check-preset-lanes.mjs"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 023 \
  --prompt prompts/preset-curator-weekly.md \
  --subagent preset-lane-curator \
  --subagent-prompt "Preset and prompt-params curator. Read .cursor/agents/disklordz-preset-lane-curator.md first." \
  "$@"
