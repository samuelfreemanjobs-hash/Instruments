from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Protocol


class _RunsDB(Protocol):
    def list_runs(self, project_id: int, limit: int = 20) -> list[dict[str, Any]]: ...


def package_root() -> Path:
    return Path(__file__).resolve().parents[1]


def load_certification_rules() -> dict[str, Any]:
    path = package_root() / "testing" / "certification-v1.json"
    return json.loads(path.read_text(encoding="utf-8"))


def load_recent_run_flags(project_id: int, db: _RunsDB) -> tuple[bool, bool, bool]:
    """Return (build_ok, test_ok, audio_ok) from the newest run of each kind.

    Missing kinds default to False (fail closed) so certify cannot pass
    without evidence in the runs table.
    """
    flags = {"build": False, "test": False, "test-audio": False}
    seen: set[str] = set()
    for run in db.list_runs(project_id, limit=50):
        kind = run.get("kind")
        if kind in flags and kind not in seen:
            flags[kind] = bool(run.get("ok"))
            seen.add(kind)
        if len(seen) == 3:
            break
    return flags["build"], flags["test"], flags["test-audio"]


def evaluate_certification(
    *,
    build_ok: bool,
    artefact_ok: bool,
    pluginval_ok: bool,
    audio_ok: bool,
) -> dict[str, Any]:
    """Evaluate hard gates from testing/certification-v1.json.

    Contract: report["passed"] is True only when every required gate passes.
    """
    rules = load_certification_rules()
    values = {
        "build_ok": build_ok,
        "artefact_ok": artefact_ok,
        "pluginval_ok": pluginval_ok,
        "audio_ok": audio_ok,
    }
    gates: list[dict[str, Any]] = []
    passed = True
    for gate in rules.get("hard_gates", []):
        field = gate["field"]
        required = bool(gate.get("required", True))
        ok = bool(values.get(field, False))
        if required and not ok:
            passed = False
        gates.append(
            {
                "id": gate.get("id", field),
                "field": field,
                "required": required,
                "ok": ok,
                "description": gate.get("description", ""),
            }
        )
    return {
        "schema_version": rules.get("schema_version", "certification/v1"),
        "passed": passed,
        "gates": gates,
        "inputs": values,
    }
