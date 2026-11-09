#!/usr/bin/env python3
"""MeterTap factory smoke — stdlib only (Windows + Linux).

Talks to a RUNNING factory at VST_FACTORY_URL (default http://127.0.0.1:8790).
Does NOT call /scaffold, /build, or /certify unless --through-scaffold is set
(APC lifecycle requires Dream/Plan/Design before scaffold writes vst/).
"""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any

DEFAULT_URL = "http://127.0.0.1:8790"
PLUGIN_NAME = "MeterTap"
PLUGIN_CLASS = "effect"
PRODUCT_CATEGORY = "fx.metering"


def _repo_plugins() -> Path:
    # scripts/ -> vst-factory/ -> disklordz/ -> Instruments/
    return Path(__file__).resolve().parents[3] / "plugins" / "MeterTap"


def _default_prompt() -> str:
    dogfood = Path(__file__).resolve().parents[1] / "data" / "meter-tap-dogfood.json"
    if dogfood.is_file():
        try:
            data = json.loads(dogfood.read_text(encoding="utf-8"))
            if data.get("prompt"):
                return str(data["prompt"])
        except (OSError, json.JSONDecodeError):
            pass
    brief = _repo_plugins() / ".ideas" / "creative-brief.md"
    if brief.is_file():
        return (
            "DiskLordz factory dogfood: stereo peak and RMS meter effect "
            "with input trim and gui.meter; uses factory-lib analysis.peak_rms only."
        )
    return (
        "DiskLordz factory dogfood: stereo peak and RMS meter effect "
        "with dl_mix_level gain and gui.meter; uses factory-lib analysis.peak_rms only."
    )


def _request(
    base: str,
    method: str,
    path: str,
    body: dict[str, Any] | None = None,
) -> tuple[int, Any]:
    url = base.rstrip("/") + path
    data = None
    headers = {"Accept": "application/json"}
    if body is not None:
        data = json.dumps(body).encode("utf-8")
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            raw = resp.read().decode("utf-8", errors="replace")
            code = resp.getcode() or 200
    except urllib.error.HTTPError as exc:
        raw = exc.read().decode("utf-8", errors="replace")
        code = exc.code
    except OSError as exc:
        return 0, {"detail": str(exc)}
    try:
        parsed: Any = json.loads(raw) if raw else None
    except json.JSONDecodeError:
        parsed = {"detail": raw}
    return code, parsed


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="MeterTap VST Factory smoke flow")
    parser.add_argument(
        "--through-scaffold",
        action="store_true",
        help="Also POST /scaffold (writes plugins/ + vst/). Off by default.",
    )
    parser.add_argument(
        "--url",
        default=os.environ.get("VST_FACTORY_URL", DEFAULT_URL),
        help="Factory base URL (env VST_FACTORY_URL)",
    )
    args = parser.parse_args(argv)
    base = args.url.rstrip("/")
    failures: list[str] = []

    print(f"MeterTap smoke → {base}")

    code, health = _request(base, "GET", "/api/health")
    if code != 200 or not isinstance(health, dict) or health.get("status") != "ok":
        failures.append(f"health: HTTP {code} {health}")
        print("FAIL: /api/health")
        _summary(failures)
        return 1
    print(f"OK  health version={health.get('version')}")

    prompt = _default_prompt()
    code, created = _request(
        base,
        "POST",
        "/api/projects",
        {
            "prompt": prompt,
            "plugin_class": PLUGIN_CLASS,
            "plugin_name": PLUGIN_NAME,
            "product_category": PRODUCT_CATEGORY,
        },
    )
    project_id: int | None = None
    if code == 200 and isinstance(created, dict):
        project_id = int(created["id"])
        print(f"OK  created project id={project_id}")
    elif code == 409:
        code, projects = _request(base, "GET", "/api/projects?limit=100")
        if code != 200 or not isinstance(projects, list):
            failures.append(f"list projects after 409: HTTP {code}")
        else:
            match = next((p for p in projects if p.get("plugin_name") == PLUGIN_NAME), None)
            if not match:
                failures.append("409 conflict but MeterTap not in /api/projects")
            else:
                project_id = int(match["id"])
                print(f"OK  reused existing project id={project_id} (409)")
    else:
        failures.append(f"create project: HTTP {code} {created}")

    if project_id is None:
        _summary(failures)
        return 1

    code, approvals = _request(base, "GET", f"/api/projects/{project_id}/approvals")
    if code != 200 or not isinstance(approvals, list):
        failures.append(f"list approvals: HTTP {code}")
        _summary(failures)
        return 1
    print(f"OK  approvals count={len(approvals)}")

    arch = next(
        (a for a in approvals if a.get("gate") == "approve_architecture" and a.get("status") == "pending"),
        None,
    )
    if arch is None:
        approved = next(
            (a for a in approvals if a.get("gate") == "approve_architecture" and a.get("status") == "approved"),
            None,
        )
        if approved:
            print(f"OK  approve_architecture already approved id={approved['id']}")
        else:
            failures.append("no approve_architecture gate found")
    else:
        code, resolved = _request(
            base,
            "POST",
            f"/api/projects/{project_id}/approvals/{arch['id']}/resolve",
            {"status": "approved"},
        )
        if code != 200:
            failures.append(f"resolve architecture: HTTP {code} {resolved}")
        else:
            print(f"OK  approved gate approve_architecture id={arch['id']}")

    if args.through_scaffold:
        code, scaff = _request(base, "POST", f"/api/projects/{project_id}/scaffold")
        if code != 200:
            failures.append(f"scaffold: HTTP {code} {scaff}")
        else:
            print(f"OK  scaffold vst_path={scaff.get('vst_path') if isinstance(scaff, dict) else scaff}")

    code, portfolio = _request(base, "GET", "/api/portfolio")
    if code != 200 or not isinstance(portfolio, dict):
        failures.append(f"portfolio: HTTP {code}")
    else:
        names = [p.get("plugin_name") for p in portfolio.get("products", [])]
        if PLUGIN_NAME not in names:
            failures.append("portfolio missing MeterTap")
        else:
            print("OK  portfolio includes MeterTap")

    code, taxonomy = _request(base, "GET", "/api/taxonomy")
    if code != 200 or not isinstance(taxonomy, dict):
        failures.append(f"taxonomy: HTTP {code}")
    else:
        cats = [c.get("id") for c in taxonomy.get("categories", [])]
        if PRODUCT_CATEGORY not in cats:
            failures.append("taxonomy missing fx.metering")
        else:
            print("OK  taxonomy includes fx.metering")

    return _summary(failures)


def _summary(failures: list[str]) -> int:
    if failures:
        print("FAIL MeterTap smoke:")
        for f in failures:
            print(f"  - {f}")
        return 1
    print("PASS MeterTap smoke")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
