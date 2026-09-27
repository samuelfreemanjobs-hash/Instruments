#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
bash "$DIR/check-billing-integrity.sh"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 020 \
  --prompt prompts/billing-integrity-weekly.md \
  --subagent billing-integrity \
  --subagent-prompt "Stripe + credits auditor. Read-only Stripe MCP unless user confirms writes. Read .cursor/agents/disklordz-billing-integrity.md first." \
  "$@"
