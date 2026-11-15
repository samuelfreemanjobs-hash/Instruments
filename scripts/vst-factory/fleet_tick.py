#!/usr/bin/env python3
"""Scheduled health tick for vst-factory-director + linux-vst3-dsp-engineer (audio-plugin-fleet CI)."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

AGENT_IDS = (
    "vst-factory-director",
    "linux-vst3-dsp-engineer",
)

REQUIRED_AGENT_FILES = (
    "agent.md",
    "skill.md",
    "soul.md",
)


def run(cmd: list[str], *, cwd: Path | None = None) -> None:
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=cwd or REPO, check=True)


def check_agent_trees() -> None:
    profit = REPO / "disklordz" / "agents" / "profit"
    for aid in AGENT_IDS:
        base = profit / aid
        if not base.is_dir():
            raise SystemExit(f"missing agent directory: {base}")
        for fname in REQUIRED_AGENT_FILES:
            path = base / fname
            if not path.is_file():
                raise SystemExit(f"missing {path}")
    skill_router = REPO / ".cursor/skills/linux-vst3-dsp-engineer/SKILL.md"
    if not skill_router.is_file():
        raise SystemExit(f"missing {skill_router}")
    print(f"OK: agent trees for {', '.join(AGENT_IDS)}")


def main() -> int:
    check_agent_trees()
    run(["python3", "disklordz/agents/profit/scaffold_agents.py", "--check"])
    run(["python3", "-m", "pytest", "disklordz/vst-factory/tests", "-q", "--tb=short"])
    run(["python3", "scripts/agent_memory.py", "check"])
    verify = REPO / "scripts/vst/verify-mpc-sample-vst3.sh"
    if verify.is_file():
        run(["bash", "-n", str(verify)])
    print("OK: vst-factory fleet tick")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())