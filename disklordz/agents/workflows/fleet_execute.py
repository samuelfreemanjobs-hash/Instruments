#!/usr/bin/env python3
"""Run safe, repo-local fleet role checks (PM Agent / agent-fleet-execute CI)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
FLEET = REPO / ".github" / "agents" / "fleet.json"


def run(cmd: list[str], *, cwd: Path | None = None, env: dict[str, str] | None = None) -> None:
    print("+", " ".join(cmd))
    import os

    merged = os.environ.copy()
    if env:
        merged.update(env)
    subprocess.run(cmd, cwd=cwd or REPO, check=True, env=merged)


def main() -> int:
    if not FLEET.is_file():
        print(f"Missing {FLEET}; run ./scripts/sync-disklordz-agent-fleet.sh", file=sys.stderr)
        return 1

    fleet = json.loads(FLEET.read_text(encoding="utf-8"))
    count = fleet.get("agent_count", 0)
    assert count >= 31, f"expected >=31 agents, got {count}"

    run(["python3", "disklordz/agents/profit/scaffold_agents.py", "--check"])
    run(["python3", "disklordz/agents/workflows/scaffold_workflows.py"])

    chunk = REPO / "disklordz/rag/scripts/chunk_corpus.py"
    if chunk.is_file():
        run(["python3", str(chunk)])

    import os

    base = os.environ.get("DISKLORDZ_URL", "").strip()
    if base:
        run(["bash", "disklordz/integrations/scripts/verify-integrations.sh"], env={"DISKLORDZ_URL": base})
        go_live = REPO / "disklordz/website/scripts/verify-go-live.sh"
        if go_live.is_file():
            run(["bash", str(go_live)], env={"DISKLORDZ_URL": base})
    else:
        print("SKIP site verify (set DISKLORDZ_URL or DISKLORDZ_VERIFY_BASE_URL in CI)")

    print(f"OK: fleet_execute completed for {count} agents")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
