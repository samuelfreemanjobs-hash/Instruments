from __future__ import annotations

import json
import os
import urllib.error
import urllib.request
from typing import Any


DEFAULT_CHANNEL_ID = "C0CG2AQ8SJ0"  # #disklordz-factory-dept


def _webhook_url() -> str | None:
    url = os.environ.get("VST_FACTORY_SLACK_WEBHOOK_URL", "").strip()
    return url or None


def _factory_base_url() -> str:
    return os.environ.get("VST_FACTORY_PUBLIC_URL", "http://127.0.0.1:8790").rstrip("/")


def format_gate_message(
    *,
    plugin_name: str,
    project_id: int,
    gate: str,
    status: str = "pending",
    detail: str | None = None,
) -> str:
    lines = [
        f"*VST Factory* — gate `{gate}` ({status})",
        f"Project: *{plugin_name}* (id `{project_id}`)",
        f"Open: {_factory_base_url()}/static/index.html",
        "Approve via API: "
        f"`POST /api/projects/{project_id}/approvals/{{id}}/resolve` with `{{\"status\":\"approved\"}}`",
    ]
    if detail:
        tail = detail if len(detail) < 400 else detail[:400] + "…"
        lines.append(f"Detail: ```{tail}```")
    return "\n".join(lines)


def notify_gate(
    *,
    plugin_name: str,
    project_id: int,
    gate: str,
    status: str = "pending",
    detail: str | None = None,
) -> dict[str, Any]:
    """Post to Slack incoming webhook when configured; otherwise no-op."""
    text = format_gate_message(
        plugin_name=plugin_name,
        project_id=project_id,
        gate=gate,
        status=status,
        detail=detail,
    )
    url = _webhook_url()
    if not url:
        return {"sent": False, "reason": "VST_FACTORY_SLACK_WEBHOOK_URL unset", "preview": text}
    channel = os.environ.get("VST_FACTORY_SLACK_CHANNEL_ID", DEFAULT_CHANNEL_ID)
    payload = {
        "text": text,
        "channel": channel,
    }
    req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=15) as resp:
            body = resp.read().decode("utf-8", errors="replace")
        return {"sent": True, "response": body}
    except urllib.error.HTTPError as exc:
        return {"sent": False, "reason": f"HTTP {exc.code}", "preview": text}
    except OSError as exc:
        return {"sent": False, "reason": str(exc), "preview": text}
