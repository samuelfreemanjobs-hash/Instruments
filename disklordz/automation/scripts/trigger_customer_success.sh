#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 024 \
  --prompt prompts/customer-success-weekly.md \
  --subagent customer-success \
  --subagent-prompt "See .cursor/agents/disklordz-customer-success.md" \
  "$@"
