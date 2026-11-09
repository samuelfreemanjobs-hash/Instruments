"""Contract: GET /pipeline returns all 10 stages in workflow order."""

from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.modules import load_full_lifecycle_workflow
from vst_factory.orchestrator import PIPELINE_ORDER, Orchestrator


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "pipe.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", Orchestrator(test_db))
    yield
    test_db.close()


def test_full_lifecycle_workflow_has_ten_stages() -> None:
    """Contract: full-lifecycle.json defines exactly the 10 canonical stages."""
    wf = load_full_lifecycle_workflow()
    ids = [s["id"] for s in wf["stages"]]
    assert ids == PIPELINE_ORDER
    assert len(ids) == 10


def test_pipeline_endpoint_ordering_and_blocking_gate() -> None:
    """Contract: pipeline lists 10 stages in order; architecture gate blocks scaffold."""
    client = TestClient(app)
    created = client.post(
        "/api/projects",
        json={
            "prompt": "Pipeline ordering fixture plugin with enough text here",
            "plugin_class": "effect",
            "plugin_name": "PipelineOrderPlugin",
        },
    )
    assert created.status_code == 200
    pid = created.json()["id"]

    pipe = client.get(f"/api/projects/{pid}/pipeline")
    assert pipe.status_code == 200
    body = pipe.json()
    assert body["workflow_id"] == "workflow/full-lifecycle"
    ids = [p["id"] for p in body["phases"]]
    assert ids == PIPELINE_ORDER
    assert len(body["phases"]) == 10
    assert body["blocking_gate"] == "approve_architecture"

    scaffold = next(p for p in body["phases"] if p["id"] == "scaffold")
    assert scaffold["status"] == "blocked"
    assert scaffold["gate"] == "approve_architecture"
    assert body["next_sop"]
    assert body["next_command"]
