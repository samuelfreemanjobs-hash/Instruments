from __future__ import annotations

from typing import Any

from vst_factory.certification import evaluate_certification, load_recent_run_flags


class _FakeDB:
    def __init__(self, runs: list[dict[str, Any]]) -> None:
        self._runs = runs

    def list_runs(self, project_id: int, limit: int = 20) -> list[dict[str, Any]]:
        return self._runs[:limit]


def test_evaluate_certification_fails_closed_when_any_hard_gate_false() -> None:
    """Contract: evaluate_certification.passed is False if any required gate fails."""
    report = evaluate_certification(
        build_ok=True,
        artefact_ok=True,
        pluginval_ok=True,
        audio_ok=False,
    )
    assert report["passed"] is False
    audio = next(g for g in report["gates"] if g["field"] == "audio_ok")
    assert audio["ok"] is False


def test_evaluate_certification_passes_when_all_gates_true() -> None:
    """Contract: evaluate_certification.passed is True only when all hard gates pass."""
    report = evaluate_certification(
        build_ok=True,
        artefact_ok=True,
        pluginval_ok=True,
        audio_ok=True,
    )
    assert report["passed"] is True
    assert all(g["ok"] for g in report["gates"])


def test_load_recent_run_flags_uses_newest_of_each_kind() -> None:
    """Contract: load_recent_run_flags returns latest build/test/test-audio ok flags."""
    db = _FakeDB(
        [
            {"kind": "test-audio", "ok": True},
            {"kind": "test", "ok": False},
            {"kind": "build", "ok": True},
            {"kind": "test", "ok": True},  # older — ignored
        ]
    )
    build_ok, test_ok, audio_ok = load_recent_run_flags(1, db)
    assert build_ok is True
    assert test_ok is False
    assert audio_ok is True


def test_load_recent_run_flags_defaults_missing_kinds_to_false() -> None:
    """Contract: missing run kinds fail closed (False)."""
    build_ok, test_ok, audio_ok = load_recent_run_flags(1, _FakeDB([]))
    assert (build_ok, test_ok, audio_ok) == (False, False, False)
