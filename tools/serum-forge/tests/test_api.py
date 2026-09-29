from fastapi.testclient import TestClient


def test_health():
    from serum_forge import main as main_module

    with TestClient(main_module.app) as client:
        r = client.get("/api/health")
    assert r.status_code == 200
    assert r.json()["status"] == "ok"


def test_schema_endpoint():
    from serum_forge import main as main_module

    with TestClient(main_module.app) as client:
        r = client.get("/api/schema")
    assert r.status_code == 200
    assert r.json().get("title") == "SerumSymbolPatch"


def test_prompt_pipeline():
    from serum_forge import main as main_module

    with TestClient(main_module.app) as client:
        r = client.post(
            "/api/pipeline/prompt",
            json={"prompt": "warm dusty pad", "archetype": "pad_ambient"},
        )
    assert r.status_code == 200
    body = r.json()
    assert body["patch"]["name"]
    assert body["intermediate_json"]


def test_validate_patch_rejects_bad_warp():
    from serum_forge import main as main_module

    with TestClient(main_module.app) as client:
        r = client.post(
            "/api/patch/validate",
            json={"patch": {"name": "Bad", "osc_a_warp_mode": "NotReal"}},
        )
    assert r.status_code == 422


def test_dx7_reject():
    from serum_forge import main as main_module

    with TestClient(main_module.app) as client:
        r = client.post("/api/dx7/reject", content=bytes([0xF0, 0x43]) + bytes(160))
    assert r.status_code == 200
    assert r.json()["rejected"] is True
