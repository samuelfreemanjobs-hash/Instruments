from __future__ import annotations

from typing import Any, Protocol


class _ResearchDB(Protocol):
    def add_approval(
        self,
        project_id: int,
        *,
        gate: str,
        status: str = "pending",
        detail: str | None = None,
    ) -> int: ...


def start_research(project_id: int, goal: str, db: _ResearchDB) -> dict[str, Any]:
    """Open a DSP research ticket and queue approve_dsp_promotion.

    Phase 1: no LLM — records the goal and returns a stub algorithm id.
    """
    algorithm_id = "research-" + "".join(ch if ch.isalnum() else "-" for ch in goal.lower())[:40].strip("-")
    result = {
        "schema_version": "dsp-research/v1",
        "project_id": project_id,
        "goal": goal,
        "status": "pending_promotion",
        "candidates": [
            {
                "algorithm_id": algorithm_id or "research-unnamed",
                "notes": "Stub candidate — promote after human review.",
            }
        ],
        "gate": "approve_dsp_promotion",
    }
    db.add_approval(
        project_id,
        gate="approve_dsp_promotion",
        status="pending",
        detail=goal[:2000],
    )
    return result
