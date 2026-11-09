"""Contract: scaffold/ship blocked without approvals; ship skipped/ok via build_broker."""

from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.orchestrator import Orchestrator


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "gates.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", Orchestrator(test_db))
    yield
    test_db.close()


def _create(client: TestClient, name: str) -> int:
    r = client.post(
        "/api/projects",
        json={
            "prompt": f"{name} gate enforcement fixture with enough text",
            "plugin_class": "effect",
            "plugin_name": name,
            "product_category": "fx.metering",
        },
    )
    assert r.status_code == 200
    return int(r.json()["id"])


def test_scaffold_blocked_without_architecture_approval() -> None:
    """Contract: POST /scaffold returns 403 until approve_architecture is approved."""
    client = TestClient(app)
    pid = _create(client, "GateScaffoldBlock")
    blocked = client.post(f"/api/projects/{pid}/scaffold")
    assert blocked.status_code == 403
    assert "approve_architecture" in blocked.json()["detail"]


def test_ship_blocked_without_certify_and_release(monkeypatch: pytest.MonkeyPatch) -> None:
    """Contract: POST /ship returns 403 without certify pass + approve_release."""
    client = TestClient(app)
    pid = _create(client, "GateShipBlock")

    # No certify run yet
    r = client.post(f"/api/projects/{pid}/ship")
    assert r.status_code == 403
    assert "certify" in r.json()["detail"].lower()

    # Fake a passed certify without opening release gate via certify() path —
    # insert run + ensure gate still pending/missing.
    orch = __import__("vst_factory.main", fromlist=["orch"]).orch
    orch.db.add_run(pid, "certify", True, '{"passed": true}')

    r2 = client.post(f"/api/projects/{pid}/ship")
    assert r2.status_code == 403
    assert "approve_release" in r2.json()["detail"]


def test_ship_skipped_path_when_broker_skips(monkeypatch: pytest.MonkeyPatch) -> None:
    """Contract: ship records skipped run (ok) without crashing when broker skips."""
    client = TestClient(app)
    pid = _create(client, "ShipSkipPlugin")
    orch = __import__("vst_factory.main", fromlist=["orch"]).orch
    orch.db.add_run(pid, "certify", True, '{"passed": true}')
    orch.db.add_approval(pid, gate="approve_release", status="approved")

    monkeypatch.setattr(
        "vst_factory.build_broker.run_package_ship",
        lambda name, version="1.0.0": ("skipped", True, "skipped: non-Linux host (Darwin)\n"),
    )

    res = client.post(f"/api/projects/{pid}/ship")
    assert res.status_code == 200
    body = res.json()
    assert body["ship_status"] == "skipped"
    assert body["last_ok"] is True
    assert body["state"] != "ship"  # skipped does not advance to SHIP

    runs = client.get(f"/api/projects/{pid}/runs").json()
    ship_runs = [r for r in runs if r["kind"] == "ship"]
    assert ship_runs
    assert ship_runs[0]["ok"] is True
    assert "skipped" in ship_runs[0]["output_tail"]


def test_ship_ok_path_sets_state_and_artifact(monkeypatch: pytest.MonkeyPatch, tmp_path: Path) -> None:
    """Contract: successful ship sets state=ship and records release artifact path."""
    client = TestClient(app)
    pid = _create(client, "ShipOkPlugin")
    orch = __import__("vst_factory.main", fromlist=["orch"]).orch
    orch.db.add_run(pid, "certify", True, '{"passed": true}')
    orch.db.add_approval(pid, gate="approve_release", status="approved")

    artifact = tmp_path / "release" / "ShipOkPlugin" / "ShipOkPlugin-1.0.0-vst3.zip"
    artifact.parent.mkdir(parents=True)
    artifact.write_bytes(b"PK\x03\x04fake")

    monkeypatch.setattr(
        "vst_factory.build_broker.run_package_ship",
        lambda name, version="1.0.0": ("ok", True, f"Package: {artifact}\n"),
    )
    monkeypatch.setattr(
        "vst_factory.orchestrator.find_release_artifact",
        lambda name: artifact,
    )

    res = client.post(f"/api/projects/{pid}/ship")
    assert res.status_code == 200
    body = res.json()
    assert body["ship_status"] == "ok"
    assert body["state"] == "ship"
    assert body["artifact"]
    assert "ShipOkPlugin" in body["artifact"]
