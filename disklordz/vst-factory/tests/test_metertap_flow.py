from __future__ import annotations

from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.orchestrator import Orchestrator


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """Isolate factory SQLite for end-to-end MeterTap API flow."""
    path = tmp_path / "metertap.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setenv("VST_FACTORY_DB", str(path))
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", Orchestrator(test_db))
    yield
    test_db.close()


def test_metertap_e2e_create_approve_portfolio_taxonomy() -> None:
    """Contract: MeterTap create → approve_architecture → portfolio/taxonomy without scaffold."""
    client = TestClient(app)

    health = client.get("/api/health")
    assert health.status_code == 200
    assert health.json()["status"] == "ok"

    created = client.post(
        "/api/projects",
        json={
            "prompt": (
                "DiskLordz factory dogfood: stereo peak and RMS meter effect "
                "with dl_mix_level gain and gui.meter; uses factory-lib analysis.peak_rms only."
            ),
            "plugin_class": "effect",
            "plugin_name": "MeterTap",
            "product_category": "fx.metering",
        },
    )
    assert created.status_code == 200, created.text
    body = created.json()
    assert body["plugin_name"] == "MeterTap"
    assert body["spec"]["product_category"] == "fx.metering"
    assert body["spec"]["dsp_modules"] == ["analysis.peak_rms"]
    pid = body["id"]

    # Second create must 409 (reuse path in smoke script)
    again = client.post(
        "/api/projects",
        json={
            "prompt": "DiskLordz factory dogfood: stereo peak and RMS meter again long enough",
            "plugin_class": "effect",
            "plugin_name": "MeterTap",
            "product_category": "fx.metering",
        },
    )
    assert again.status_code == 409

    # Scaffold blocked until architecture gate approved
    blocked = client.post(f"/api/projects/{pid}/scaffold")
    assert blocked.status_code == 403

    approvals = client.get(f"/api/projects/{pid}/approvals").json()
    pending = next(a for a in approvals if a["gate"] == "approve_architecture")
    assert pending["status"] == "pending"
    resolved = client.post(
        f"/api/projects/{pid}/approvals/{pending['id']}/resolve",
        json={"status": "approved"},
    )
    assert resolved.status_code == 200
    assert client.get(f"/api/projects/{pid}/approvals").json()[0]["status"] == "approved"

    portfolio = client.get("/api/portfolio")
    assert portfolio.status_code == 200
    products = portfolio.json()["products"]
    mt = next(p for p in products if p["plugin_name"] == "MeterTap")
    assert mt["factory_project"]["id"] == pid

    taxonomy = client.get("/api/taxonomy")
    assert taxonomy.status_code == 200
    ids = [c["id"] for c in taxonomy.json()["categories"]]
    assert "fx.metering" in ids

    wo = client.get(f"/api/projects/{pid}/product-wo")
    assert wo.status_code == 200
    assert wo.json()["plugin_name"] == "MeterTap"

    # Slack webhook unset → create still succeeds (no-op notify)
    assert "VST_FACTORY_SLACK_WEBHOOK_URL" not in __import__("os").environ or not __import__(
        "os"
    ).environ.get("VST_FACTORY_SLACK_WEBHOOK_URL")
