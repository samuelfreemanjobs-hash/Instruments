from __future__ import annotations

import hashlib
import hmac
import json
import time
import uuid
from typing import Any

import httpx
from pydantic import BaseModel, Field

from coding_agent_stack.config import get_settings
from coding_agent_stack.grok_bridge import DelegateRequest, build_handoff
from coding_agent_stack.schema import ImplementationPlan


class GrokAcpTask(BaseModel):
    """Agent Client Protocol–shaped payload for Grok Bot ingress."""

    protocol: str = "acp/0.1"
    message_type: str = "task"
    channel_id: str = ""
    text: str = Field(max_length=32_000)
    routine_id: str | None = None
    client_ref: str = ""
    context: dict[str, Any] = Field(default_factory=dict)


class GrokSendResult(BaseModel):
    ok: bool
    dry_run: bool = False
    status_code: int = 0
    request_id: str = ""
    detail: str = ""
    payload: dict[str, Any] = Field(default_factory=dict)


def load_agents_profile(max_chars: int = 24_000) -> str:
    settings = get_settings()
    rel = settings.agents_profile_rel
    path = (settings.repo_root / rel).resolve()
    root = settings.repo_root.resolve()
    if not str(path).startswith(str(root)) or not path.is_file():
        return ""
    text = path.read_text(encoding="utf-8", errors="replace")
    if len(text) > max_chars:
        return text[:max_chars] + "\n\n...[truncated for ACP context]"
    return text


def load_routines() -> dict[str, str]:
    settings = get_settings()
    raw = settings.grok_routines_json.strip()
    if not raw:
        return {}
    try:
        data = json.loads(raw)
        if isinstance(data, dict):
            return {str(k): str(v) for k, v in data.items()}
    except json.JSONDecodeError:
        pass
    return {}


def build_acp_payload(
    prompt: str,
    *,
    routine_id: str | None = None,
    client_ref: str = "",
    plan_json: str | None = None,
    include_agents_md: bool = True,
) -> GrokAcpTask:
    settings = get_settings()
    ctx: dict[str, Any] = {
        "repo_root": str(settings.repo_root),
        "harness_hint": "vst-testing-ops/run_business.py --profile ci",
    }
    if include_agents_md:
        profile = load_agents_profile()
        if profile:
            ctx["agents_md"] = profile
    if plan_json:
        ctx["architect_plan_json"] = plan_json

    plan = None
    if plan_json:
        try:
            plan = ImplementationPlan.model_validate_json(plan_json)
        except ValueError:
            plan = None
    handoff = build_handoff(DelegateRequest(target="grok", user_prompt=prompt, plan=plan))
    ctx["outer_loop_brief"] = handoff.brief_markdown[:16_000]

    return GrokAcpTask(
        channel_id=settings.grok_channel_id,
        text=prompt,
        routine_id=routine_id,
        client_ref=client_ref or str(uuid.uuid4()),
        context=ctx,
    )


async def send_to_grok_channel(
    prompt: str,
    *,
    routine_id: str | None = None,
    client_ref: str = "",
    plan_json: str | None = None,
) -> GrokSendResult:
    settings = get_settings()
    task = build_acp_payload(
        prompt,
        routine_id=routine_id,
        client_ref=client_ref,
        plan_json=plan_json,
    )
    payload = task.model_dump()
    request_id = task.client_ref

    url = settings.grok_webhook_url.strip()
    if not url:
        return GrokSendResult(
            ok=False,
            dry_run=True,
            request_id=request_id,
            detail="GROK_WEBHOOK_URL not set — configure Grok Bot ACP ingress",
            payload=payload,
        )

    headers = {"Content-Type": "application/json", "X-ACP-Protocol": task.protocol}
    token = settings.grok_bot_token.strip()
    if token:
        headers["Authorization"] = f"Bearer {token}"

    try:
        async with httpx.AsyncClient(timeout=settings.grok_http_timeout_s) as client:
            resp = await client.post(url, json=payload, headers=headers)
    except httpx.HTTPError as exc:
        return GrokSendResult(
            ok=False,
            request_id=request_id,
            detail="Upstream request failed",
            payload=payload,
        )

    ok = 200 <= resp.status_code < 300
    detail = resp.text[:500] if resp.text else ""
    if not ok:
        detail = "Grok ingress returned an error"
    return GrokSendResult(
        ok=ok,
        dry_run=False,
        status_code=resp.status_code,
        request_id=request_id,
        detail=detail,
        payload=payload,
    )


def verify_grok_webhook_signature(body: bytes, signature_header: str | None) -> bool:
    settings = get_settings()
    secret = settings.grok_webhook_secret.strip()
    if not secret:
        return True
    if not signature_header:
        return False
    expected = hmac.new(secret.encode(), body, hashlib.sha256).hexdigest()
    provided = signature_header.removeprefix("sha256=").strip()
    return hmac.compare_digest(expected, provided)


def parse_inbound_event(body: dict[str, Any]) -> dict[str, Any]:
    """Normalize Grok → PWA webhook callbacks."""
    return {
        "received_at": time.time(),
        "protocol": body.get("protocol", "acp/0.1"),
        "message_type": body.get("message_type", "event"),
        "client_ref": body.get("client_ref", ""),
        "text": str(body.get("text", ""))[:16_000],
        "status": body.get("status", "unknown"),
        "raw_keys": list(body.keys())[:32],
    }
