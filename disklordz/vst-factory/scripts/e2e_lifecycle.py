#!/usr/bin/env python3
"""Drive a plugin through the full VST Factory API lifecycle (10 stages).

Talks to a RUNNING factory (default http://127.0.0.1:8790). Intended for
dogfood SKUs that already have plugins/<Name>/ + vst/<Name>/ (scaffold is
idempotent and reuses existing trees).

Example:
  VST_FACTORY_DB=/tmp/factory-e2e.sqlite \\
    uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790 &
  python3 disklordz/vst-factory/scripts/e2e_lifecycle.py MeterTap \\
    --evidence plugins/MeterTap/evidence/factory-e2e.json
"""

from __future__ import annotations

import argparse
import json
import os
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any

DEFAULT_URL = "http://127.0.0.1:8790"
PIPELINE_STAGE_IDS = (
    "intake",
    "dream",
    "plan",
    "design",
    "scaffold",
    "impl",
    "build",
    "test",
    "certify",
    "ship",
)


def _request(
    base: str,
    method: str,
    path: str,
    body: dict[str, Any] | None = None,
    timeout: float = 3600.0,
) -> tuple[int, Any, float]:
    url = base.rstrip("/") + path
    data = None
    headers = {"Accept": "application/json"}
    if body is not None:
        data = json.dumps(body).encode("utf-8")
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    t0 = time.perf_counter()
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            raw = resp.read().decode("utf-8", errors="replace")
            code = resp.getcode() or 200
    except urllib.error.HTTPError as exc:
        raw = exc.read().decode("utf-8", errors="replace")
        code = exc.code
    except OSError as exc:
        return 0, {"detail": str(exc)}, time.perf_counter() - t0
    elapsed = time.perf_counter() - t0
    try:
        parsed: Any = json.loads(raw) if raw else None
    except json.JSONDecodeError:
        parsed = {"detail": raw}
    return code, parsed, elapsed


def _find_pending(approvals: list[dict[str, Any]], gate: str) -> dict[str, Any] | None:
    for a in approvals:
        if a.get("gate") == gate and a.get("status") == "pending":
            return a
    return None


def _approve(base: str, pid: int, gate: str, transcript: list[dict[str, Any]]) -> bool:
    code, approvals, dt = _request(base, "GET", f"/api/projects/{pid}/approvals")
    transcript.append(
        {"method": "GET", "path": f"/api/projects/{pid}/approvals", "status": code, "dt_s": dt, "body": approvals}
    )
    if code != 200 or not isinstance(approvals, list):
        return False
    pending = _find_pending(approvals, gate)
    if pending is None:
        # Already approved?
        for a in approvals:
            if a.get("gate") == gate and a.get("status") == "approved":
                transcript.append({"note": f"gate {gate} already approved id={a.get('id')}"})
                return True
        transcript.append({"error": f"no pending/approved gate {gate}", "approvals": approvals})
        return False
    aid = pending["id"]
    code2, resolved, dt2 = _request(
        base, "POST", f"/api/projects/{pid}/approvals/{aid}/resolve", {"status": "approved"}
    )
    transcript.append(
        {
            "method": "POST",
            "path": f"/api/projects/{pid}/approvals/{aid}/resolve",
            "status": code2,
            "dt_s": dt2,
            "request": {"status": "approved"},
            "body": resolved,
            "gate": gate,
        }
    )
    return code2 == 200


def run_lifecycle(
    plugin_name: str,
    *,
    base: str,
    plugin_class: str = "effect",
    product_category: str = "fx.metering",
    prompt: str | None = None,
) -> dict[str, Any]:
    transcript: list[dict[str, Any]] = []
    failures: list[str] = []
    prompt = prompt or (
        f"DiskLordz factory e2e: {plugin_name} — full lifecycle through ship."
    )

    code, health, dt = _request(base, "GET", "/api/health", timeout=30)
    transcript.append({"method": "GET", "path": "/api/health", "status": code, "dt_s": dt, "body": health})
    if code != 200:
        failures.append(f"health HTTP {code}")
        return {"ok": False, "failures": failures, "transcript": transcript}

    create_body = {
        "prompt": prompt,
        "plugin_class": plugin_class,
        "plugin_name": plugin_name,
        "product_category": product_category,
    }
    code, proj, dt = _request(base, "POST", "/api/projects", create_body, timeout=60)
    transcript.append(
        {
            "method": "POST",
            "path": "/api/projects",
            "status": code,
            "dt_s": dt,
            "request": create_body,
            "body": proj,
        }
    )
    if code != 200 or not isinstance(proj, dict) or "id" not in proj:
        failures.append(f"create project HTTP {code}: {proj}")
        return {"ok": False, "failures": failures, "transcript": transcript}
    pid = int(proj["id"])

    if not _approve(base, pid, "approve_architecture", transcript):
        failures.append("approve_architecture failed")

    code, synced, dt = _request(base, "POST", f"/api/projects/{pid}/sync", timeout=120)
    transcript.append(
        {"method": "POST", "path": f"/api/projects/{pid}/sync", "status": code, "dt_s": dt, "body": synced}
    )
    if code != 200:
        failures.append(f"sync after architecture HTTP {code}")

    code, scaff, dt = _request(base, "POST", f"/api/projects/{pid}/scaffold", timeout=300)
    transcript.append(
        {"method": "POST", "path": f"/api/projects/{pid}/scaffold", "status": code, "dt_s": dt, "body": scaff}
    )
    if code != 200:
        failures.append(f"scaffold HTTP {code}: {scaff}")

    # Design is already present for dogfood; sync opens approve_gui.
    code, synced2, dt = _request(base, "POST", f"/api/projects/{pid}/sync", timeout=120)
    transcript.append(
        {
            "method": "POST",
            "path": f"/api/projects/{pid}/sync",
            "status": code,
            "dt_s": dt,
            "body": synced2,
            "note": "open approve_gui",
        }
    )
    if not _approve(base, pid, "approve_gui", transcript):
        failures.append("approve_gui failed")

    code, synced3, dt = _request(base, "POST", f"/api/projects/{pid}/sync", timeout=120)
    transcript.append(
        {
            "method": "POST",
            "path": f"/api/projects/{pid}/sync",
            "status": code,
            "dt_s": dt,
            "body": synced3,
            "note": "accept impl",
        }
    )

    for kind in ("build", "test", "certify"):
        code, body, dt = _request(base, "POST", f"/api/projects/{pid}/{kind}", timeout=3600)
        transcript.append(
            {
                "method": "POST",
                "path": f"/api/projects/{pid}/{kind}",
                "status": code,
                "dt_s": dt,
                "body": body,
            }
        )
        if code != 200:
            failures.append(f"{kind} HTTP {code}: {body}")
        elif kind == "certify" and isinstance(body, dict) and not body.get("passed"):
            failures.append(f"certify not passed: {body}")
        elif kind in ("build", "test") and isinstance(body, dict) and body.get("last_ok") is False:
            failures.append(f"{kind} last_ok=false")

    if not _approve(base, pid, "approve_release", transcript):
        failures.append("approve_release failed")

    code, shipped, dt = _request(base, "POST", f"/api/projects/{pid}/ship", timeout=3600)
    transcript.append(
        {"method": "POST", "path": f"/api/projects/{pid}/ship", "status": code, "dt_s": dt, "body": shipped}
    )
    if code != 200:
        failures.append(f"ship HTTP {code}: {shipped}")
    elif isinstance(shipped, dict) and shipped.get("ship_status") not in ("ok",):
        failures.append(f"ship_status={shipped.get('ship_status')}")

    code, pipeline, dt = _request(base, "GET", f"/api/projects/{pid}/pipeline", timeout=60)
    transcript.append(
        {"method": "GET", "path": f"/api/projects/{pid}/pipeline", "status": code, "dt_s": dt, "body": pipeline}
    )

    stages_done: list[str] = []
    if code == 200 and isinstance(pipeline, dict):
        phases = pipeline.get("phases") or pipeline.get("stages") or []
        for ph in phases:
            sid = ph.get("id")
            st = ph.get("status")
            if sid in PIPELINE_STAGE_IDS and st == "done":
                stages_done.append(sid)
        missing = [s for s in PIPELINE_STAGE_IDS if s not in stages_done]
        if missing:
            failures.append(f"pipeline stages not done: {missing}")
    else:
        failures.append(f"pipeline HTTP {code}")

    return {
        "ok": len(failures) == 0,
        "plugin_name": plugin_name,
        "project_id": pid,
        "stages_done": stages_done,
        "failures": failures,
        "transcript": transcript,
        "pipeline": pipeline if code == 200 else None,
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="VST Factory full-lifecycle e2e")
    parser.add_argument("plugin_name", help="Plugin name (e.g. MeterTap)")
    parser.add_argument("--url", default=os.environ.get("VST_FACTORY_URL", DEFAULT_URL))
    parser.add_argument("--class", dest="plugin_class", default="effect")
    parser.add_argument("--category", dest="product_category", default="fx.metering")
    parser.add_argument(
        "--evidence",
        type=Path,
        default=None,
        help="Write full request/response transcript JSON here",
    )
    args = parser.parse_args(argv)

    result = run_lifecycle(
        args.plugin_name,
        base=args.url.rstrip("/"),
        plugin_class=args.plugin_class,
        product_category=args.product_category,
    )
    if args.evidence:
        args.evidence.parent.mkdir(parents=True, exist_ok=True)
        args.evidence.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(f"Wrote evidence → {args.evidence}")

    print(json.dumps({"ok": result["ok"], "stages_done": result.get("stages_done"), "failures": result.get("failures")}, indent=2))
    return 0 if result["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
