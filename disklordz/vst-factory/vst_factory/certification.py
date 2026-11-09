from __future__ import annotations

import json
from pathlib import Path
from typing import Any

def cert_schema() -> dict[str, Any]:
    path = Path(__file__).resolve().parents[1] / "testing/certification-v1.json"
    return json.loads(path.read_text(encoding="utf-8"))


def evaluate_certification(
    *,
    build_ok: bool,
    artefact_ok: bool,
    pluginval_ok: bool,
    audio_ok: bool | None = None,
    runs: list[dict[str, Any]] | None = None,
) -> dict[str, Any]:
    schema = cert_schema()
    hard: dict[str, bool] = {
        "build_succeeds": build_ok,
        "vst3_artefact_exists": artefact_ok,
        "pluginval_pass": pluginval_ok,
        "no_crash_on_load": pluginval_ok,
        "no_nan_audio_output": audio_ok if audio_ok is not None else True,
    }
    hard_fail = [k for k, v in hard.items() if not v]
    audio_gate = audio_ok if audio_ok is not None else True
    gates = [
        {"field": "build_ok", "ok": build_ok},
        {"field": "artefact_ok", "ok": artefact_ok},
        {"field": "pluginval_ok", "ok": pluginval_ok},
        {"field": "audio_ok", "ok": audio_gate},
    ]
    weights = schema["weights"]
    scores: dict[str, float] = {
        "build_reliability": 1.0 if build_ok else 0.0,
        "audio_correctness": 1.0 if (audio_ok is not False) else 0.0,
        "stability": 1.0 if pluginval_ok else 0.0,
        "cpu_performance": 0.5,
        "midi_functionality": 0.5,
        "parameter_state": 0.5,
        "gui_functionality": 0.5,
        "documentation": 0.5,
    }
    weighted = sum(scores[k] * weights[k] for k in weights)
    passed = len(hard_fail) == 0
    return {
        "passed": passed,
        "gates": gates,
        "hard_gates": hard,
        "hard_failures": hard_fail,
        "weighted_score": round(weighted, 4),
        "category_scores": scores,
        "minimum_for_rc": schema.get("minimum_score_for_release_candidate", 0.7),
    }


def _latest_run_ok(runs: list[dict[str, Any]], kind: str) -> bool:
    for r in runs:
        if r["kind"] == kind:
            return bool(r["ok"])
    return False


def load_recent_run_flags(project_id: int, db: Any) -> tuple[bool, bool, bool]:
    runs = db.list_runs(project_id, limit=30)
    if not runs:
        return False, False, False
    return (
        _latest_run_ok(runs, "build"),
        _latest_run_ok(runs, "test"),
        _latest_run_ok(runs, "test-audio"),
    )
