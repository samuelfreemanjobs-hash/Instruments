#!/usr/bin/env bash
# Queue a Cursor Cloud Agent task via GitHub (creates a labeled issue).
# Use from OpenClaw/Telegram glue, local shell, or Zapier "Webhooks by Zapier".
set -euo pipefail

REPO="${GITHUB_REPO:-samuelfreemanjobs-hash/Instruments}"
SOURCE="${1:-telegram}"
TASK="${2:-}"

usage() {
  cat <<'EOF'
Usage:
  trigger-agent-task.sh <source> "<task markdown>"
  trigger-agent-task.sh manual "Fix golden WAV drift on program 48"

Sources: telegram | slack | linear | openclaw | manual

Requires: gh auth login with repo scope (or GITHUB_TOKEN).
EOF
}

if [[ -z "$TASK" ]]; then
  usage
  exit 1
fi

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || { echo "missing: $1" >&2; exit 1; }
}

need_cmd gh
need_cmd jq

gh api --method POST "repos/${REPO}/dispatches" \
  -f event_type=agent-task \
  -f "client_payload[task]=${TASK}" \
  -f "client_payload[source]=${SOURCE}"

echo "Queued agent-task on ${REPO} (source=${SOURCE}). Check Actions → Cloud agent task."
