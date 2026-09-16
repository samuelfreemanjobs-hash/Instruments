#!/usr/bin/env python3
"""Provision Disklordz factory webhook flow on self-hosted Activepieces (API)."""

from __future__ import annotations

import json
import os
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
AP_DIR = REPO_ROOT / "infra" / "activepieces"
WEBHOOK_FILE = AP_DIR / ".disklordz-webhook-url"
FLOW_DISPLAY_NAME = "Disklordz — factory batch"
WEBHOOK_PIECE = "@activepieces/piece-webhook"
WEBHOOK_TRIGGER = "catch_webhook"


def _load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip().strip('"').strip("'")
        os.environ.setdefault(key, value)


def _api_base() -> str:
    frontend = os.environ.get("AP_FRONTEND_URL", "http://localhost:8080").rstrip("/")
    return f"{frontend}/api"


def _request(
    method: str,
    path: str,
    *,
    token: str | None = None,
    body: dict | None = None,
    params: dict | None = None,
) -> tuple[int, dict | list | str]:
    url = _api_base() + path
    if params:
        qs = urllib.parse.urlencode({k: v for k, v in params.items() if v is not None}, doseq=True)
        url = f"{url}?{qs}"
    data = None
    headers = {"Accept": "application/json"}
    if body is not None:
        data = json.dumps(body).encode("utf-8")
        headers["Content-Type"] = "application/json"
    if token:
        headers["Authorization"] = f"Bearer {token}"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            raw = resp.read().decode("utf-8")
            if not raw:
                return resp.status, {}
            return resp.status, json.loads(raw)
    except urllib.error.HTTPError as exc:
        raw = exc.read().decode("utf-8", errors="replace")
        try:
            payload = json.loads(raw) if raw else {}
        except json.JSONDecodeError:
            payload = raw
        return exc.code, payload


def _wait_for_health(timeout_sec: int = 120) -> None:
    deadline = time.time() + timeout_sec
    while time.time() < deadline:
        code, body = _request("GET", "/v1/health/")
        if code == 200 and isinstance(body, dict) and body.get("status") == "Healthy":
            return
        time.sleep(2)
    raise SystemExit("Activepieces health check timed out (is docker compose up?)")


def _sign_up_or_sign_in(email: str, password: str, first: str, last: str) -> tuple[str, str]:
    signup_body = {
        "email": email,
        "password": password,
        "firstName": first,
        "lastName": last,
        "trackEvents": False,
        "newsLetter": False,
    }
    code, body = _request("POST", "/v1/authentication/sign-up", body=signup_body)
    if code in (200, 201) and isinstance(body, dict) and body.get("token"):
        project_id = body.get("projectId")
        if not project_id:
            raise SystemExit("Sign-up succeeded but projectId missing — finish setup in UI once.")
        return body["token"], project_id

    signin_body = {"email": email, "password": password}
    code, body = _request("POST", "/v1/authentication/sign-in", body=signin_body)
    if code == 200 and isinstance(body, dict) and body.get("token"):
        project_id = body.get("projectId")
        if not project_id:
            raise SystemExit("Sign-in OK but projectId missing.")
        return body["token"], project_id

    raise SystemExit(f"Authentication failed (sign-up {code}): {body}")


def _piece_version(token: str, project_id: str) -> str:
    code, body = _request("GET", "/v1/pieces", token=token, params={"projectId": project_id})
    if code != 200:
        return "0.1.41"
    items = body if isinstance(body, list) else body.get("data", []) if isinstance(body, dict) else []
    for piece in items:
        if piece.get("name") == WEBHOOK_PIECE:
            return str(piece.get("version", "0.1.41"))
    return "0.1.41"


def _find_flow(token: str, project_id: str) -> str | None:
    code, body = _request(
        "GET",
        "/v1/flows",
        token=token,
        params={"projectId": project_id, "limit": "50", "name": FLOW_DISPLAY_NAME},
    )
    if code != 200:
        return None
    rows = body.get("data", []) if isinstance(body, dict) else body
    for row in rows:
        if row.get("displayName") == FLOW_DISPLAY_NAME or row.get("version", {}).get("displayName") == FLOW_DISPLAY_NAME:
            return row["id"]
    return None


def _create_flow(token: str, project_id: str) -> str:
    code, body = _request(
        "POST",
        "/v1/flows",
        token=token,
        body={"displayName": FLOW_DISPLAY_NAME, "projectId": project_id},
        params={"projectId": project_id},
    )
    if code not in (200, 201):
        raise SystemExit(f"Create flow failed ({code}): {body}")
    return body["id"]


def _flow_op(token: str, flow_id: str, op_type: str, request: dict) -> dict:
    code, body = _request(
        "POST",
        f"/v1/flows/{flow_id}",
        token=token,
        body={"type": op_type, "request": request},
    )
    if code != 200:
        raise SystemExit(f"Flow operation {op_type} failed ({code}): {body}")
    return body


def _slack_code(slack_url: str) -> str:
    escaped = json.dumps(slack_url)
    return f"""export const code = async (inputs) => {{
  const trigger = inputs.trigger ?? {{}};
  const body = trigger.body ?? trigger;
  const kit = body.kit_name ?? "unknown";
  const count = body.count ?? 0;
  const batch = body.batch_id ?? "";
  const text = `Disklordz factory: ${{kit}} — ${{count}} WAVs (batch ${{batch}})`;
  const url = {escaped};
  if (!url) return {{ notified: false, text }};
  const res = await fetch(url, {{
    method: "POST",
    headers: {{ "Content-Type": "application/json" }},
    body: JSON.stringify({{ text }}),
  }});
  return {{ notified: res.ok, text }};
}};
"""


def _webhook_url(flow_id: str) -> str:
    frontend = os.environ.get("AP_FRONTEND_URL", "http://localhost:8080").rstrip("/")
    return f"{frontend}/api/v1/webhooks/{flow_id}"


def _provision_flow(token: str, project_id: str, slack_url: str) -> str:
    existing = _find_flow(token, project_id)
    if existing:
        print(f"Flow already exists ({existing}); skipping graph edits.")
        return _webhook_url(existing)

    flow_id = _create_flow(token, project_id)
    piece_version = _piece_version(token, project_id)

    trigger = {
        "type": "PIECE",
        "name": "trigger",
        "displayName": "Factory webhook",
        "valid": True,
        "settings": {
            "pieceName": WEBHOOK_PIECE,
            "pieceVersion": piece_version,
            "triggerName": WEBHOOK_TRIGGER,
            "input": {"authType": "none"},
            "propertySettings": {},
        },
    }
    _flow_op(token, flow_id, "UPDATE_TRIGGER", trigger)

    if slack_url:
        action = {
            "parentStep": "trigger",
            "action": {
                "type": "CODE",
                "name": "step_slack",
                "displayName": "Notify Slack",
                "valid": True,
                "skip": False,
                "settings": {
                    "input": {},
                    "sourceCode": {
                        "code": _slack_code(slack_url),
                        "packageJson": "{}",
                    },
                },
            },
        }
        _flow_op(token, flow_id, "ADD_ACTION", action)

    _flow_op(token, flow_id, "LOCK_AND_PUBLISH", {"status": "ENABLED"})
    return _webhook_url(flow_id)


def main() -> int:
    _load_dotenv(AP_DIR / ".env")
    email = os.environ.get("DISKLORDZ_AP_ADMIN_EMAIL", "").strip()
    password = os.environ.get("DISKLORDZ_AP_ADMIN_PASSWORD", "").strip()
    if not email or not password:
        print(
            "Skip flow provision: set DISKLORDZ_AP_ADMIN_EMAIL and DISKLORDZ_AP_ADMIN_PASSWORD in infra/activepieces/.env",
            file=sys.stderr,
        )
        return 0

    first = os.environ.get("DISKLORDZ_AP_ADMIN_FIRST_NAME", "Disklordz").strip() or "Disklordz"
    last = os.environ.get("DISKLORDZ_AP_ADMIN_LAST_NAME", "Studio").strip() or "Studio"
    slack_url = os.environ.get("DISKLORDZ_SLACK_WEBHOOK", "").strip()

    print("Waiting for Activepieces…")
    _wait_for_health()
    print("Signing in…")
    token, project_id = _sign_up_or_sign_in(email, password, first, last)
    print(f"Provisioning flow '{FLOW_DISPLAY_NAME}'…")
    webhook_url = _provision_flow(token, project_id, slack_url)
    WEBHOOK_FILE.write_text(webhook_url + "\n")
    print(f"Webhook URL → {WEBHOOK_FILE}")
    print(webhook_url)
    print("Factory picks this up automatically (or set DISKLORDZ_ACTIVEPIECES_WEBHOOK).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
