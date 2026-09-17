#!/usr/bin/env bash
# Post DiskLordz Factory agent check-in to Slack (all core agents).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
API="${FACTORY_API_URL:-http://127.0.0.1:8787}"
FACTORY_API="$ROOT/disklordz-factory/apps/factory-api"
PY="${FACTORY_API}/.venv/bin/python3"

if curl -fsS "${API}/health" >/dev/null 2>&1; then
  echo "Posting via Factory API ${API}/factory/slack/checkin ..."
  curl -fsS -X POST "${API}/factory/slack/checkin" | python3 -m json.tool
  exit 0
fi

if [[ ! -x "$PY" ]]; then
  python3 -m venv "${FACTORY_API}/.venv"
  "$PY" -m pip install -q -r "${FACTORY_API}/requirements.txt"
fi

echo "Factory API not up; posting directly ..."
"$PY" -c "
import sys, json
sys.path.insert(0, '$FACTORY_API')
from disklordz_factory import slack_agents
print(json.dumps(slack_agents.notify_agent_checkin(), indent=2))
"
