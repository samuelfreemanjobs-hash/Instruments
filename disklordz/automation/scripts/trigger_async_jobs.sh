#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 026 \
  --prompt prompts/async-jobs-audit.md \
  --subagent async-jobs \
  --subagent-prompt "See .cursor/agents/disklordz-async-jobs.md" \
  "$@"
