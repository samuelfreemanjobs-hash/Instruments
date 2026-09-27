#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec bash "$DIR/trigger_cursor_business_agent.sh" \
  --wo 027 \
  --prompt prompts/audio-qa-golden.md \
  --subagent audio-qa \
  --subagent-prompt "See .cursor/agents/disklordz-audio-qa.md" \
  "$@"
