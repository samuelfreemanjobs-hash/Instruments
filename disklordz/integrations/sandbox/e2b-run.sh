#!/usr/bin/env bash
# Run a command inside E2B when E2B_API_KEY is set (https://github.com/e2b-dev/E2B)
set -euo pipefail
if [[ -z "${E2B_API_KEY:-}" ]]; then
  echo "Set E2B_API_KEY to use E2B sandboxes."
  exit 1
fi
python3 - <<'PY'
import os, subprocess, sys
try:
    from e2b_code_interpreter import Sandbox
except ImportError:
    print("pip install e2b-code-interpreter", file=sys.stderr)
    sys.exit(1)
cmd = os.environ.get("E2B_CMD", "python3 disklordz/rag/scripts/chunk_corpus.py")
with Sandbox() as sb:
    r = sb.commands.run(cmd)
    print(r.stdout)
    if r.stderr:
        print(r.stderr, file=sys.stderr)
PY
