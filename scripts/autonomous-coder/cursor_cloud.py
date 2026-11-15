"""Cursor Cloud / Background Agents API — launch and dedupe."""

from __future__ import annotations

import base64
import json
import os
import urllib.error
import urllib.request
from typing import Any

from agent_rules import agent_rules_block

API_V0 = "https://api.cursor.com/v0/agents"
MAX_CONCURRENT = 3

RUNNING_STATUSES = frozenset(
    {
        "RUNNING",
        "ACTIVE",
        "CREATING",
        "PENDING",
        "QUEUED",
        "IN_PROGRESS",
    }
)


def api_key() -> str | None:
    key = (os.environ.get("CURSOR_API_KEY") or "").strip()
    return key or None


def _auth_header(key: str) -> str:
    token = base64.b64encode(f"{key}:".encode()).decode()
    return f"Basic {token}"


def _request(
    method: str,
    url: str,
    key: str,
    payload: dict | None = None,
    *,
    bearer: bool = False,
) -> tuple[int, Any]:
    data = None
    headers = {"Content-Type": "application/json", "Accept": "application/json"}
    if bearer:
        headers["Authorization"] = f"Bearer {key}"
    else:
        headers["Authorization"] = _auth_header(key)
    if payload is not None:
        data = json.dumps(payload).encode("utf-8")
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            body = resp.read().decode("utf-8")
            return resp.status, json.loads(body) if body else {}
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        try:
            parsed = json.loads(body) if body else {"error": body}
        except json.JSONDecodeError:
            parsed = {"error": body}
        return exc.code, parsed


def list_agents(key: str) -> list[dict]:
    """List cloud agents (v0)."""
    status, data = _request("GET", API_V0, key, bearer=True)
    if status == 401:
        status, data = _request("GET", API_V0, key, bearer=False)
    if status >= 400:
        print(f"cursor list agents HTTP {status}: {data}")
        return []
    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        for key_name in ("agents", "data", "items"):
            if isinstance(data.get(key_name), list):
                return data[key_name]
        if isinstance(data.get("agent"), dict):
            return [data["agent"]]
    return []


def agent_status(agent: dict) -> str:
    return str(agent.get("status") or agent.get("state") or "").upper()


def agent_text_blob(agent: dict) -> str:
    parts = [
        str(agent.get("name") or ""),
        str(agent.get("id") or ""),
        str(agent.get("summary") or ""),
    ]
    prompt = agent.get("prompt")
    if isinstance(prompt, dict):
        parts.append(str(prompt.get("text") or ""))
    elif isinstance(prompt, str):
        parts.append(prompt)
    return "\n".join(parts)


def count_running(agents: list[dict]) -> int:
    return sum(1 for a in agents if agent_status(a) in RUNNING_STATUSES)


def slice_already_running(agents: list[dict], slice_id: str) -> bool:
    needle = slice_id.lower()
    for agent in agents:
        if agent_status(agent) not in RUNNING_STATUSES:
            continue
        blob = agent_text_blob(agent).lower()
        if needle in blob or f"queue id:` `{needle}" in blob.replace(" ", ""):
            return True
    return False


def build_prompt(item: dict, slice_md: str, *, pr: dict | None) -> str:
    pr_block = ""
    if pr:
        pr_block = (
            f"\n**GitHub PR:** #{pr.get('number')} — {pr.get('title', '')}\n"
            f"**Branch:** `{pr.get('headRefName', 'main')}` (base `{pr.get('baseRefName', 'main')}`)\n"
        )
    return f"""Autonomous coding slice — execute end-to-end in this repository.

**Queue id:** `{item['id']}`
**Title:** {item.get('title', '')}
{pr_block}
{agent_rules_block()}

---

## NEXT_SLICE.md

{slice_md}
"""


def create_agent(
    key: str,
    *,
    prompt_text: str,
    repo_url: str,
    ref: str,
    auto_create_pr: bool = True,
) -> tuple[str | None, dict]:
    """POST /v0/agents. Returns (agent_id, raw_response)."""
    payload_v0 = {
        "prompt": {"text": prompt_text},
        "source": {"repository": repo_url, "ref": ref},
        "autoCreatePr": auto_create_pr,
    }
    status, data = _request("POST", API_V0, key, payload_v0, bearer=True)
    if status in (404, 400, 422):
        payload_v1 = {
            "prompt": {"text": prompt_text},
            "repos": [{"url": repo_url, "startingRef": ref}],
            "autoCreatePR": auto_create_pr,
        }
        status, data = _request(
            "POST", "https://api.cursor.com/v1/agents", key, payload_v1, bearer=False
        )
    if status >= 400:
        print(f"cursor create agent HTTP {status}: {data}")
        return None, data
    agent = data.get("agent") if isinstance(data, dict) else None
    if not agent and isinstance(data, dict):
        agent = data
    agent_id = None
    if isinstance(agent, dict):
        agent_id = agent.get("id") or agent.get("agentId")
    return agent_id, data


def launch_slice_agent(
    item: dict,
    slice_md: str,
    *,
    repo_url: str,
    ref: str,
    pr: dict | None,
    state: dict,
) -> bool:
    """Launch Cursor Cloud Agent if policy allows. Mutates state."""
    key = api_key()
    if not key:
        print("SKIP launch: CURSOR_API_KEY not set (add repo secret — docs/AUTOMATION.md)")
        return False

    agents = list_agents(key)
    running = count_running(agents)
    if running >= MAX_CONCURRENT:
        print(f"SKIP launch: {running} agents already running (max {MAX_CONCURRENT})")
        return False

    slice_id = item["id"]
    if slice_already_running(agents, slice_id):
        print(f"SKIP launch: agent already running for slice {slice_id}")
        return False

    prompt_text = build_prompt(item, slice_md, pr=pr)
    agent_id, raw = create_agent(
        key,
        prompt_text=prompt_text,
        repo_url=repo_url,
        ref=ref,
        auto_create_pr=True,
    )
    if not agent_id:
        return False

    launched = state.setdefault("launched_agents", {})
    launched[slice_id] = {
        "agent_id": agent_id,
        "pr": pr.get("number") if pr else item.get("pr"),
        "ref": ref,
        "raw_status": agent_status(raw.get("agent", raw)) if isinstance(raw, dict) else "",
    }
    print(f"Launched Cursor agent {agent_id} for slice {slice_id}")
    return True