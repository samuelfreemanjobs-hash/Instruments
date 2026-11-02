import os
from pathlib import Path

import pytest
from fastapi.testclient import TestClient


@pytest.fixture()
def client(tmp_path: Path):
    os.environ["SYNTH_FORGE_DB_PATH"] = str(tmp_path / "api.db")
    from synth_forge import main as main_module

    with TestClient(main_module.app) as c:
        yield c
    main_module.memory.close()


def test_health(client: TestClient):
    r = client.get("/api/health")
    assert r.status_code == 200
    assert r.json()["status"] == "ok"


def test_synths_list(client: TestClient):
    r = client.get("/api/synths")
    assert r.status_code == 200
    ids = {s["id"] for s in r.json()}
    assert "minifreak" in ids and "zenology" in ids


def test_batch_generate_api(client: TestClient):
    r = client.post(
        "/api/batch/generate",
        json={"synth_id": "minilogue_xd", "count": 2, "category": "g_funk_whistle"},
    )
    assert r.status_code == 200
    body = r.json()
    assert len(body["presets"]) == 2


def test_prompt_api(client: TestClient):
    r = client.post(
        "/api/prompt/generate",
        json={"prompt": "Cardo luxury pad", "synth_id": "ultranova", "count": 2},
    )
    assert r.status_code == 200
    assert len(r.json()["presets"]) == 2


def test_memory_vault(client: TestClient):
    client.post("/api/batch/generate", json={"synth_id": "dx7", "count": 1})
    r = client.get("/api/memory")
    assert r.status_code == 200
    assert len(r.json()) >= 1
