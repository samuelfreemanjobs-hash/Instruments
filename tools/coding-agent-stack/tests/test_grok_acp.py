import pytest
from fastapi.testclient import TestClient

from coding_agent_stack.grok_acp import build_acp_payload, load_agents_profile
from coding_agent_stack.main import app


def test_agents_md_loaded():
    text = load_agents_profile()
    assert "JUCE" in text or "Agents" in text


def test_acp_payload_has_agents_context():
    task = build_acp_payload("hello grok", client_ref="t1")
    assert task.protocol == "acp/0.1"
    assert "agents_md" in task.context or task.context.get("repo_root")


def test_grok_send_dry_run():
    client = TestClient(app)
    r = client.post("/grok/send", json={"prompt": "Monitor PR 107", "architect_first": False})
    assert r.status_code in (202, 502)
    data = r.json()
    assert "dry_run" in data


def test_grok_webhook_unsigned_ok_without_secret():
    client = TestClient(app)
    r = client.post(
        "/grok/webhook",
        json={"message_type": "event", "text": "done", "client_ref": "t1", "status": "ok"},
    )
    assert r.status_code == 200
    st = client.get("/grok/status")
    assert st.json().get("last_event") is not None
