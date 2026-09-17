#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
python3 -m venv "$DIR/.venv"
"$DIR/.venv/bin/pip" install -r "$DIR/requirements.txt"
echo "OK: use $DIR/.venv/bin/python3 server.py"
