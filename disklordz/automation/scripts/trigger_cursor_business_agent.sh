#!/usr/bin/env bash
# Generic Cursor Cloud Agent launcher for Disklordz business agents (WO-SAAS-019+).
set -euo pipefail

REPO_URL="${DISKLORDZ_GITHUB_REPO_URL:-https://github.com/samuelfreemanjobs-hash/Instruments}"
START_REF="${DISKLORDZ_AGENT_REF:-main}"
AUTOMATION_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DRY_RUN=0

WO=""
PROMPT_FILE=""
SUBAGENT_NAME=""
SUBAGENT_PROMPT=""
THEME=""

usage() {
  cat <<'EOF'
Usage: trigger_cursor_business_agent.sh --wo 019 --prompt prompts/saas-ops-daily.md \
  --subagent saas-ops-guardian --subagent-prompt "..." [--theme T] [--ref main] [--dry-run]
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --wo) WO="$2"; shift 2 ;;
    --prompt) PROMPT_FILE="$2"; shift 2 ;;
    --subagent) SUBAGENT_NAME="$2"; shift 2 ;;
    --subagent-prompt) SUBAGENT_PROMPT="$2"; shift 2 ;;
    --theme) THEME="$2"; shift 2 ;;
    --ref) START_REF="$2"; shift 2 ;;
    --dry-run) DRY_RUN=1; shift ;;
    -h | --help) usage; exit 0 ;;
    *) echo "unknown arg: $1" >&2; exit 1 ;;
  esac
done

[[ -n "$WO" && -n "$PROMPT_FILE" && -n "$SUBAGENT_NAME" ]] || {
  usage >&2
  exit 1
}

if [[ "$PROMPT_FILE" != /* ]]; then
  PROMPT_FILE="$AUTOMATION_DIR/$PROMPT_FILE"
fi
[[ -f "$PROMPT_FILE" ]] || { echo "missing prompt: $PROMPT_FILE" >&2; exit 1; }

PROMPT_TEXT="$(cat "$PROMPT_FILE")"
PROMPT_TEXT="${PROMPT_TEXT//\{\{THEME\}\}/${THEME:-general}}"
PROMPT_TEXT="${PROMPT_TEXT//\{\{WO\}\}/WO-SAAS-${WO}}"
if [[ -n "${PM_ROUTER_INTAKE:-}" ]]; then
  PROMPT_TEXT="${PROMPT_TEXT//\{\{INTAKE\}\}/${PM_ROUTER_INTAKE}}"
fi
if [[ -n "${PM_ROUTER_CLASSIFY:-}" ]]; then
  PROMPT_TEXT="${PROMPT_TEXT//\{\{CLASSIFY\}\}/${PM_ROUTER_CLASSIFY}}"
fi

if [[ "$DRY_RUN" -eq 1 ]]; then
  echo "=== DRY RUN WO-SAAS-${WO} ref=${START_REF} theme=${THEME:-} ==="
  printf '%s\n' "$PROMPT_TEXT"
  exit 0
fi

if [[ -z "${CURSOR_API_KEY:-}" ]]; then
  echo "CURSOR_API_KEY not set — skip Cloud Agent launch (WO-SAAS-${WO})."
  exit 0
fi

command -v curl >/dev/null && command -v jq >/dev/null || {
  echo "missing curl or jq" >&2
  exit 1
}

payload="$(jq -n \
  --arg text "$PROMPT_TEXT" \
  --arg url "$REPO_URL" \
  --arg ref "$START_REF" \
  --arg wo "$WO" \
  --arg theme "${THEME:-}" \
  --arg subName "$SUBAGENT_NAME" \
  --arg subPrompt "${SUBAGENT_PROMPT:-Read the agent charter in .cursor/agents/}" \
  '{
    prompt: { text: $text },
    mode: "agent",
    repos: [{ url: $url, startingRef: $ref }],
    autoCreatePR: true,
    envVars: {
      WO_SAAS: $wo,
      AGENT_THEME: $theme
    },
    customSubagents: [{
      name: $subName,
      description: ("Disklordz business agent WO-SAAS-" + $wo),
      prompt: $subPrompt
    }]
  }')"

echo "Launching Cursor Cloud Agent WO-SAAS-${WO}..."
http_code="$(curl -sS -o /tmp/cursor-business-agent.json -w "%{http_code}" \
  -u "${CURSOR_API_KEY}:" \
  -H "Content-Type: application/json" \
  -X POST "https://api.cursor.com/v1/agents" \
  -d "$payload")"

if [[ "$http_code" != "200" && "$http_code" != "201" ]]; then
  echo "Cursor API HTTP ${http_code}" >&2
  head -c 800 /tmp/cursor-business-agent.json >&2 || true
  exit 1
fi

jq '{agent: .agent.id, run: .run.id, status: .run.status}' /tmp/cursor-business-agent.json
