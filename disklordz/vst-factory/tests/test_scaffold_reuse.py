"""Contract: scaffold reuses existing plugins/<Name> + vst/<Name> without error."""

from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.orchestrator import Orchestrator


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "scaffold-reuse.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", Orchestrator(test_db))
    yield
    test_db.close()


def test_scaffold_reuses_existing_metertap_trees(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """Contract: when APC + VST trees exist, POST /scaffold returns 200 and marks reuse."""
    root = Path(__file__).resolve().parents[3]
    # MeterTap dogfood trees must exist in the real repo for this assertion.
    assert (root / "plugins" / "MeterTap").is_dir()
    assert (root / "vst" / "MeterTap").is_dir()

    client = TestClient(app)
    created = client.post(
        "/api/projects",
        json={
            "prompt": "MeterTap scaffold reuse fixture with enough text for intake",
            "plugin_class": "effect",
            "plugin_name": "MeterTap",
            "product_category": "fx.metering",
        },
    )
    assert created.status_code == 200, created.text
    pid = created.json()["id"]

    approvals = client.get(f"/api/projects/{pid}/approvals").json()
    arch = next(a for a in approvals if a["gate"] == "approve_architecture")
    assert client.post(
        f"/api/projects/{pid}/approvals/{arch['id']}/resolve",
        json={"status": "approved"},
    ).status_code == 200

    scaff = client.post(f"/api/projects/{pid}/scaffold")
    assert scaff.status_code == 200, scaff.text
    body = scaff.json()
    assert body.get("vst_path")
    assert "MeterTap" in body["vst_path"]
    # Spec flag set by orchestrator
    assert body.get("spec", {}).get("scaffold_reused") is True
