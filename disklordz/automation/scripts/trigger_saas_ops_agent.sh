#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 019 \
  --prompt prompts/saas-ops-daily.md \
  --subagent saas-ops-guardian \
  --subagent-prompt "SRE for Disklordz Next.js: health, go-live, API smoke, env docs. Read .cursor/agents/disklordz-saas-ops-guardian.md first." \
  "$@"
