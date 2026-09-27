#!/usr/bin/env bash
# WO-SAAS-022: classify intake and launch the matching agent (or PM router Cloud Agent).
set -euo pipefail

DIR="$(cd "$(dirname "$0")" && pwd)"
MODE="auto"
INTAKE_TEXT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --agent) MODE="agent"; shift ;;
    --file) INTAKE_TEXT="$(cat "$2")"; shift 2 ;;
    *) INTAKE_TEXT+="$1 "; shift ;;
  esac
done

if [[ -z "$INTAKE_TEXT" ]]; then
  INTAKE_TEXT="$(cat)"
fi

CLASSIFY="$(printf '%s' "$INTAKE_TEXT" | node "$DIR/pm-router-classify.mjs")"
echo "$CLASSIFY"

if [[ "$MODE" == "agent" ]]; then
  export PM_ROUTER_CLASSIFY="$CLASSIFY"
  export PM_ROUTER_INTAKE="$INTAKE_TEXT"
  exec bash "$DIR/trigger_cursor_business_agent.sh" \
    --wo 022 \
    --prompt prompts/pm-router-dispatch.md \
    --subagent pm-wo-router \
    --subagent-prompt "Route work to the correct Disklordz lane and open draft PRs. Read .cursor/agents/disklordz-pm-wo-router.md first."
fi

LANE="$(printf '%s' "$CLASSIFY" | node -e "let d='';process.stdin.on('data',c=>d+=c);process.stdin.on('end',()=>console.log(JSON.parse(d).primaryLane))")"

case "$LANE" in
  factory_dsp) exec bash "$DIR/trigger_cursor_factory_daily_agent.sh" ;;
  billing) exec bash "$DIR/trigger_billing_agent.sh" ;;
  growth) exec bash "$DIR/trigger_growth_agent.sh" ;;
  preset_curator) exec bash "$DIR/trigger_preset_curator_agent.sh" ;;
  saas_ops | *) exec bash "$DIR/trigger_saas_ops_agent.sh" ;;
esac
