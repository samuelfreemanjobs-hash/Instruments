#!/usr/bin/env bash
# PM Agent + Workflow Automation — sync repo-wide fleet docs and checks.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

python3 disklordz/agents/profit/scaffold_agents.py --publish-skills
python3 disklordz/agents/workflows/scaffold_workflows.py
python3 disklordz/agents/profit/scaffold_agents.py --check
python3 scripts/update-roadmap-progress.py

echo "Fleet synced:"
echo "  - DISKLORDZ_AGENTS.md"
echo "  - docs/DISKLORDZ_AGENT_FLEET.md"
echo "  - .github/agents/fleet.json"
echo "  - disklordz/agents/workflows/PM_ADD.md"
