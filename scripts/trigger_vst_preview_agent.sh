#!/usr/bin/env bash
# Launch Cursor Cloud Agent for VST preview orchestration (WO-PLUGIN-001).
set -euo pipefail
REPO_URL="${DISKLORDZ_GITHUB_REPO_URL:-https://github.com/samuelfreemanjobs-hash/Instruments}"
START_REF="${DISKLORDZ_AGENT_REF:-main}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROMPT_FILE="$ROOT/vst-testing-ops/automation/prompts/vst-preview-run.md"
DRY_RUN=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --ref) START_REF="$2"; shift 2 ;;
    --dry-run) DRY_RUN=1; shift ;;
    -h | --help)
      echo "Usage: $0 [--ref main] [--dry-run]"
      exit 0
      ;;
    *) echo "unknown: $1" >&2; exit 1 ;;
  esac
done

[[ -f "$PROMPT_FILE" ]] || { echo "missing $PROMPT_FILE" >&2; exit 1; }
PROMPT_TEXT="$(cat "$PROMPT_FILE")"

if [[ "$DRY_RUN" -eq 1 ]]; then
  echo "=== DRY RUN WO-PLUGIN-001 ref=$START_REF ==="
  printf '%s\n' "$PROMPT_TEXT"
  exit 0
fi

if [[ -z "${CURSOR_API_KEY:-}" ]]; then
  echo "CURSOR_API_KEY not set — run locally: ./scripts/instruments-vst-preview.sh"
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
  '{
    prompt: { text: $text },
    mode: "agent",
    repos: [{ url: $url, startingRef: $ref }],
    autoCreatePR: false,
    customSubagents: [{
      name: "vst-preview",
      description: "WO-PLUGIN-001 VST preview orchestrator",
      prompt: "Read .cursor/agents/instruments-vst-preview-agent.md first."
    }]
  }')"

http_code="$(curl -sS -o /tmp/cursor-vst-preview-agent.json -w "%{http_code}" \
  -u "${CURSOR_API_KEY}:" \
  -H "Content-Type: application/json" \
  -X POST "https://api.cursor.com/v1/agents" \
  -d "$payload")"

if [[ "$http_code" != "200" && "$http_code" != "201" ]]; then
  echo "Cursor API HTTP ${http_code}" >&2
  head -c 800 /tmp/cursor-vst-preview-agent.json >&2 || true
  exit 1
fi

jq '{agent: .agent.id, run: .run.id, status: .run.status}' /tmp/cursor-vst-preview-agent.json
