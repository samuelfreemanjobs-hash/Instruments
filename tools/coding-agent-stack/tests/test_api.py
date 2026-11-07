from fastapi.testclient import TestClient

from coding_agent_stack.main import app


def test_health():
    client = TestClient(app)
    r = client.get("/health")
    assert r.status_code == 200
    assert r.json()["ok"] is True


def test_agent_stub():
    client = TestClient(app)
    r = client.post("/agent", json={"prompt": "run agent tests", "force_stub": True})
    assert r.status_code == 200
    body = r.json()
    assert body["mode"] == "stub"
    assert body["tool_calls"] >= 1
