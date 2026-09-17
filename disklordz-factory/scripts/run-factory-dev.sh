#!/usr/bin/env bash
# Start Factory API + dashboard for local operator UI.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
API_DIR="$ROOT/apps/factory-api"
UI_DIR="$ROOT/apps/dashboard"

if [[ ! -d "$API_DIR/.venv" ]]; then
  python3 -m venv "$API_DIR/.venv"
  "$API_DIR/.venv/bin/pip" install -r "$API_DIR/requirements.txt"
fi

if [[ ! -d "$UI_DIR/node_modules" ]]; then
  (cd "$UI_DIR" && npm install)
fi

SESSION="disklordz-factory-dev"
tmux -f /exec-daemon/tmux.portal.conf has-session -t "=$SESSION" 2>/dev/null && tmux -f /exec-daemon/tmux.portal.conf kill-session -t "$SESSION" || true
tmux -f /exec-daemon/tmux.portal.conf new-session -d -s "$SESSION" -c "$API_DIR" -- "${SHELL:-bash}" -l
tmux -f /exec-daemon/tmux.portal.conf split-window -h -t "$SESSION" -c "$UI_DIR"
tmux -f /exec-daemon/tmux.portal.conf send-keys -t "$SESSION:0.0" ".venv/bin/uvicorn disklordz_factory.main:app --host 127.0.0.1 --port 8787 --reload" C-m
tmux -f /exec-daemon/tmux.portal.conf send-keys -t "$SESSION:0.1" "npm run dev -- --host 127.0.0.1" C-m
echo "Factory API: http://127.0.0.1:8787/docs"
echo "Dashboard:   http://127.0.0.1:5173"
