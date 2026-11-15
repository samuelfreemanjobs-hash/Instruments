from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app, db, orch


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "test.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", __import__("vst_factory.orchestrator", fromlist=["Orchestrator"]).Orchestrator(test_db))
    yield
    test_db.close()


def test_health() -> None:
    client = TestClient(app)
    r = client.get("/api/health")
    assert r.status_code == 200
    assert r.json()["status"] == "ok"


def test_create_project() -> None:
    client = TestClient(app)
    r = client.post(
        "/api/projects",
        json={
            "prompt": "Vintage Memphis chord generator with sixteen pads",
            "plugin_class": "midi",
            "plugin_name": "MemphisPadsDemo",
        },
    )
    assert r.status_code == 200
    data = r.json()
    assert data["plugin_name"] == "MemphisPadsDemo"
    assert data["spec"]["dsp_modules"]
