"""Contract: phase sync reads fixture status.json and advances factory state."""

from __future__ import annotations

import json
from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from vst_factory.apc_bridge import infer_apc_phase, read_status
from vst_factory.db import FactoryDB
from vst_factory.main import app
from vst_factory.orchestrator import Orchestrator


@pytest.fixture(autouse=True)
def tmp_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    path = tmp_path / "sync.sqlite"
    test_db = FactoryDB(path)
    monkeypatch.setattr("vst_factory.main.db", test_db)
    monkeypatch.setattr("vst_factory.main.orch", Orchestrator(test_db))
    yield
    test_db.close()


def _write_status(plugins_root: Path, name: str, validation: dict, phase: str = "dream") -> Path:
    plugin = plugins_root / name
    (plugin / ".ideas").mkdir(parents=True)
    status = {
        "plugin_name": name,
        "current_phase": phase,
        "apc_phase": phase,
        "validation": validation,
    }
    (plugin / "status.json").write_text(json.dumps(status, indent=2), encoding="utf-8")
    return plugin


def test_read_status_and_sync_advances_to_design(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    """Contract: sync_phase moves state to design when architecture is verified."""
    root = tmp_path / "repo"
    plugins = root / "plugins"
    name = "SyncDesignPlugin"
    plugin = _write_status(
        plugins,
        name,
        {
            "creative_brief_exists": True,
            "parameter_spec_exists": True,
            "architecture_defined": True,
            "design_complete": False,
        },
        phase="design",
    )
    (plugin / ".ideas" / "creative-brief.md").write_text("# brief\n", encoding="utf-8")
    (plugin / ".ideas" / "parameter-spec.md").write_text("# params\n", encoding="utf-8")
    (plugin / ".ideas" / "architecture.md").write_text("# arch\n", encoding="utf-8")

    monkeypatch.setenv("VST_FACTORY_REPO_ROOT", str(root))
    # Avoid depending on tools/apc-gate.sh in the fixture tree.
    monkeypatch.setattr(
        "vst_factory.apc_bridge.run_apc_gate",
        lambda plugin_name, phase, root=None: (phase in ("plan", "design"), "ok"),
    )

    status = read_status(name, root=root)
    assert status is not None
    assert status["validation"]["architecture_defined"] is True

    apc_phase, validation = infer_apc_phase(name, root=root)
    assert apc_phase == "design"
    assert validation["architecture_defined"] is True

    client = TestClient(app)
    created = client.post(
        "/api/projects",
        json={
            "prompt": "Sync design fixture plugin with enough characters",
            "plugin_class": "effect",
            "plugin_name": name,
        },
    )
    assert created.status_code == 200
    pid = created.json()["id"]

    synced = client.post(f"/api/projects/{pid}/sync")
    assert synced.status_code == 200
    body = synced.json()
    assert body["state"] == "design"
    assert body["phase"] == "design"
    assert body["validation"]["architecture_defined"] is True

    approvals = client.get(f"/api/projects/{pid}/approvals").json()
    gates = {a["gate"] for a in approvals}
    assert "approve_gui" in gates


def test_sync_caps_impl_without_approve_gui(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    """Contract: impl completion is not accepted until approve_gui is approved."""
    root = tmp_path / "repo"
    name = "SyncImplCap"
    plugin = _write_status(
        root / "plugins",
        name,
        {
            "creative_brief_exists": True,
            "parameter_spec_exists": True,
            "architecture_defined": True,
            "design_complete": True,
        },
        phase="impl",
    )
    ideas = plugin / ".ideas"
    for fname in ("creative-brief.md", "parameter-spec.md", "architecture.md", "design-spec.md"):
        (ideas / fname).write_text(f"# {fname}\n", encoding="utf-8")

    monkeypatch.setenv("VST_FACTORY_REPO_ROOT", str(root))
    monkeypatch.setattr(
        "vst_factory.apc_bridge.run_apc_gate",
        lambda plugin_name, phase, root=None: (True, "ok"),
    )

    client = TestClient(app)
    pid = client.post(
        "/api/projects",
        json={
            "prompt": "Impl cap fixture plugin needs enough prompt text",
            "plugin_class": "effect",
            "plugin_name": name,
        },
    ).json()["id"]

    body = client.post(f"/api/projects/{pid}/sync").json()
    assert body["state"] in ("design", "dream", "plan")
    assert body["state"] != "impl"

    gui = next(a for a in client.get(f"/api/projects/{pid}/approvals").json() if a["gate"] == "approve_gui")
    client.post(
        f"/api/projects/{pid}/approvals/{gui['id']}/resolve",
        json={"status": "approved"},
    )
    body2 = client.post(f"/api/projects/{pid}/sync").json()
    assert body2["state"] == "impl"
