from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def package_root() -> Path:
    return Path(__file__).resolve().parents[1]


def list_algorithms() -> list[dict[str, Any]]:
    alg_dir = package_root() / "knowledge/dsp/algorithms"
    if not alg_dir.is_dir():
        return []
    out: list[dict[str, Any]] = []
    for f in sorted(alg_dir.glob("*.json")):
        out.append(json.loads(f.read_text(encoding="utf-8")))
    return out


def start_research(project_id: int, goal: str, db: Any) -> dict[str, Any]:
    candidates = list_algorithms()
    matched = [c for c in candidates if goal.lower() in json.dumps(c).lower()] or candidates[:1]
    db.add_approval(
        project_id,
        gate="approve_dsp_promotion",
        status="pending",
        detail=json.dumps({"goal": goal, "candidates": matched}),
    )
    return {"goal": goal, "candidates": matched, "approval": "approve_dsp_promotion"}
