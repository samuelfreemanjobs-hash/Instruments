from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.slack_notify import notify_gate


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "test.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", __import__("vst_factory.orchestrator", fromlist=["Orchestrator"]).Orchestrator(test_db))
    yield
    test_db.close()


def test_slack_notify_no_webhook() -> None:
    out = notify_gate(plugin_name="MeterTap", project_id=1, gate="approve_architecture")
    assert out["sent"] is False
    assert "preview" in out


def test_approval_flow_blocks_scaffold() -> None:
    client = TestClient(app)
    r = client.post(
        "/api/projects",
        json={
            "prompt": "MeterTap dogfood peak RMS meter",
            "plugin_class": "effect",
            "plugin_name": "MeterTapApprovalTest",
            "product_category": "fx.metering",
        },
    )
    assert r.status_code == 200
    pid = r.json()["id"]
    r2 = client.post(f"/api/projects/{pid}/scaffold")
    assert r2.status_code == 403
    approvals = client.get(f"/api/projects/{pid}/approvals").json()
    pending = [a for a in approvals if a["status"] == "pending"][0]
    client.post(
        f"/api/projects/{pid}/approvals/{pending['id']}/resolve",
        json={"status": "approved"},
    )
    assert client.get(f"/api/projects/{pid}/approvals").json()[0]["status"] == "approved"
