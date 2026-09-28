#!/usr/bin/env python3
"""Refresh auto-generated progress block in docs/ROADMAP.md."""

from __future__ import annotations

import json
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
ROADMAP = REPO / "docs" / "ROADMAP.md"
BEGIN = "<!-- ROADMAP_PROGRESS_BEGIN -->"
END = "<!-- ROADMAP_PROGRESS_END -->"


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> None:
    integrations = load_json(REPO / "disklordz/integrations/manifest.json")
    status = Counter(r["status"] for r in integrations["repos"])
    fleet = load_json(REPO / ".github/agents/fleet.json")
    activation = Counter(a["activation"] for a in fleet["agents"])

    active_ci = sum(
        1
        for a in fleet["agents"]
        if a["activation"].startswith("ci_") or a["activation"] in ("runtime_api", "runtime_inngest", "runtime_build")
    )

    block = f"""{BEGIN}
*Last updated:* {datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC")} · regenerate: `python3 scripts/update-roadmap-progress.py`

| Metric | Value |
|--------|------:|
| **Profit agents (documented)** | {fleet["agent_count"]} |
| **Agents with CI or runtime activation** | {active_ci} / {fleet["agent_count"]} |
| **OSS integrations (manifest)** | {len(integrations["repos"])} total · {status.get("integrated", 0)} integrated · {status.get("partial", 0)} partial · {status.get("external", 0)} external |
| **Fleet autopilot** | `agent-fleet-governance.yml` · `agent-fleet-execute.yml` · `agent-fleet-health.yml` (on `main` after merge) |

**Agent activation breakdown:** {", ".join(f"`{k}` ×{v}" for k, v in sorted(activation.items()))}
{END}"""

    text = ROADMAP.read_text(encoding="utf-8")
    if BEGIN not in text:
        raise SystemExit(f"Missing {BEGIN} in {ROADMAP}")
    pre, rest = text.split(BEGIN, 1)
    _, post = rest.split(END, 1)
    ROADMAP.write_text(pre + block + post, encoding="utf-8")
    print("Updated docs/ROADMAP.md progress block")


if __name__ == "__main__":
    main()
