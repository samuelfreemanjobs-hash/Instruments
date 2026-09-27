#!/usr/bin/env bash
# Launch Cursor Cloud Agent for WO-SAAS-018 daily factory improvement.
# Requires: CURSOR_API_KEY (GitHub secret or env). See docs/DISKLORDZ_FACTORY_DAILY_AGENT.md
set -euo pipefail

REPO_URL="${DISKLORDZ_GITHUB_REPO_URL:-https://github.com/samuelfreemanjobs-hash/Instruments}"
START_REF="${DISKLORDZ_FACTORY_AGENT_REF:-main}"
PROMPT_FILE="$(cd "$(dirname "$0")/.." && pwd)/prompts/factory-daily-improvement.md"
DRY_RUN=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dry-run) DRY_RUN=1; shift ;;
    --theme) export FACTORY_DAILY_THEME="$2"; shift 2 ;;
    *) echo "unknown arg: $1" >&2; exit 1 ;;
  esac
done

if [[ -z "${FACTORY_DAILY_THEME:-}" ]]; then
  case "$(date -u +%u)" in
    1) FACTORY_DAILY_THEME="kick_808_sub_click" ;;
    2) FACTORY_DAILY_THEME="snare_clap_snap" ;;
    3) FACTORY_DAILY_THEME="hats_metallic_motion" ;;
    4) FACTORY_DAILY_THEME="master_bus_lanes" ;;
    5) FACTORY_DAILY_THEME="loops_patterns" ;;
    6) FACTORY_DAILY_THEME="product_pack_consistency" ;;
    7) FACTORY_DAILY_THEME="prompt_params_rag" ;;
    *) FACTORY_DAILY_THEME="master_bus_lanes" ;;
  esac
fi

if [[ ! -f "$PROMPT_FILE" ]]; then
  echo "missing prompt file: $PROMPT_FILE" >&2
  exit 1
fi

PROMPT_TEXT="$(sed "s/{{FACTORY_DAILY_THEME}}/${FACTORY_DAILY_THEME}/g" "$PROMPT_FILE")"

if [[ "$DRY_RUN" -eq 1 ]]; then
  echo "=== DRY RUN theme=${FACTORY_DAILY_THEME} ref=${START_REF} ==="
  printf '%s\n' "$PROMPT_TEXT"
  exit 0
fi

if [[ -z "${CURSOR_API_KEY:-}" ]]; then
  echo "CURSOR_API_KEY not set — skip Cloud Agent launch (regression-only mode)."
  exit 0
fi

need_cmd() { command -v "$1" >/dev/null 2>&1 || { echo "missing: $1" >&2; exit 1; }; }
need_cmd curl
need_cmd jq

payload="$(jq -n \
  --arg text "$PROMPT_TEXT" \
  --arg url "$REPO_URL" \
  --arg ref "$START_REF" \
  --arg theme "$FACTORY_DAILY_THEME" \
  '{
    prompt: { text: $text },
    model: { id: "composer-2.5" },
    mode: "agent",
    repos: [{ url: $url, startingRef: $ref }],
    autoCreatePR: true,
    customSubagents: [{
      name: "factory-dsp-engineer",
      description: "DSP + drum sound design for disklordz/website/src/lib/generation",
      prompt: "You specialize in 808 kicks, phonk/snare snap, hat metal, master bus, and loop patterns. Read .cursor/agents/disklordz-factory-dsp-engineer.md first."
    }],
    metadata: { workOrder: "WO-SAAS-018", theme: $theme }
  }')"

echo "Launching Cursor Cloud Agent (theme=${FACTORY_DAILY_THEME})..."
http_code="$(curl -sS -o /tmp/cursor-agent-response.json -w "%{http_code}" \
  -u "${CURSOR_API_KEY}:" \
  -H "Content-Type: application/json" \
  -X POST "https://api.cursor.com/v1/agents" \
  -d "$payload")"

if [[ "$http_code" != "200" && "$http_code" != "201" ]]; then
  echo "Cursor API returned HTTP ${http_code}" >&2
  head -c 800 /tmp/cursor-agent-response.json >&2 || true
  exit 1
fi

agent_id="$(jq -r '.agent.id // .id // empty' /tmp/cursor-agent-response.json)"
run_id="$(jq -r '.run.id // empty' /tmp/cursor-agent-response.json)"
echo "Cursor agent started agentId=${agent_id} runId=${run_id}"
jq '{agent: .agent.id, run: .run.id, status: .run.status}' /tmp/cursor-agent-response.json 2>/dev/null || cat /tmp/cursor-agent-response.json
