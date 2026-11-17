#!/usr/bin/env python3
"""MPC Sample Coder agent — routing index and repo self-test (no firmware)."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]

REQUIRED_DOCS = (
    "docs/MPC_SAMPLE_ECOSYSTEM.md",
    "docs/MPC_SAMPLE_CODER.md",
)

AGENT_PLAYBOOK = "disklordz/agents/profit/mpc-sample-coder/PLAYBOOK.md"

ROUTES = {
    "xpj-generate": {
        "tool": "MPCTK (tarikcampos/MPC-Sample-Toolkit)",
        "format": "gzip JSON .xpj + _[ProjectData]",
        "device": "MPC Sample stock / Melodee import",
        "cursor_skill": "mpc-sample-xpj",
    },
    "xpm-keygroup": {
        "tool": "xpm.md + KeyGroupBuilder or MpcXpmWriter (migrate to MPCVObject)",
        "format": "MPCVObject XML + co-located .WAV",
        "device": "Live/Force/Melodee",
        "cursor_skill": "mpc-sample-xpm-keygroup",
    },
    "rust-mod": {
        "tool": "ioma8/mpc-sample-sdk",
        "format": "cdylib + autorun zip",
        "device": "Stock app LD_PRELOAD",
        "cursor_skill": "mpc-sample-rust-mod",
    },
    "virtual-lab": {
        "tool": "orrpunk-art/virtual-mpc-sample",
        "format": "QEMU + lab.py (BYO firmware)",
        "device": "Dev only",
        "cursor_skill": "mpc-sample-virtual-lab",
    },
    "sd-groovebox": {
        "tool": "keremimo/melodee-sample",
        "format": "autorun/ Melodee (AGPL)",
        "device": "Replaces stock after boot handoff",
        "cursor_skill": "mpc-sample-sd-autorun",
    },
}


def docs_ok() -> list[str]:
    missing = []
    for rel in REQUIRED_DOCS:
        if not (REPO / rel).is_file():
            missing.append(rel)
    if not (REPO / AGENT_PLAYBOOK).is_file():
        missing.append(AGENT_PLAYBOOK)
    return missing


def self_test() -> dict[str, object]:
    missing = docs_ok()
    trap_test = REPO / "disklordz/trap-lord-synth/tests/test_mpc_xpm_export.py"
    xpm_smoke: dict[str, object] = {"ran": False, "ok": False}
    if trap_test.is_file():
        proc = subprocess.run(
            [sys.executable, str(trap_test)],
            cwd=REPO,
            capture_output=True,
            text=True,
            check=False,
        )
        xpm_smoke = {
            "ran": True,
            "ok": proc.returncode == 0,
            "tail": (proc.stdout + proc.stderr).strip().splitlines()[-3:],
        }
    agent_dir = REPO / "disklordz/agents/profit/mpc-sample-coder"
    fleet_files = sorted(p.name for p in agent_dir.glob("*.md")) if agent_dir.is_dir() else []
    return {
        "ok": not missing and (not xpm_smoke["ran"] or xpm_smoke["ok"]),
        "docs_missing": missing,
        "fleet_agent_files": fleet_files,
        "xpm_schema_smoke": xpm_smoke,
        "routes": ROUTES,
    }


def suggest_route(task: str) -> str:
    t = task.lower()
    if any(k in t for k in ("xpj", "mpctk", "projectdata")):
        return "xpj-generate"
    if any(k in t for k in ("xpm", "keygroup", "mpcobject", "live", "force")):
        return "xpm-keygroup"
    if any(k in t for k in ("rust", "ld_preload", "sdk", "extract", "hook")):
        return "rust-mod"
    if any(k in t for k in ("qemu", "virtual", "lab.py", "emulator")):
        return "virtual-lab"
    if any(k in t for k in ("melodee", "groovebox", "vst3 host", "autorun")):
        return "sd-groovebox"
    return "xpm-keygroup"


def main() -> int:
    parser = argparse.ArgumentParser(description="MPC Sample Coder (fleet agent CLI)")
    parser.add_argument("--self-test", action="store_true", help="Docs + optional xpm smoke")
    parser.add_argument("--route", metavar="TASK", help="Suggest ecosystem route keyword")
    parser.add_argument("--list-routes", action="store_true")
    args = parser.parse_args()

    if args.list_routes:
        print(json.dumps(ROUTES, indent=2))
        return 0

    if args.route:
        key = suggest_route(args.route)
        print(json.dumps({"task": args.route, "route": key, **ROUTES[key]}, indent=2))
        return 0

    if args.self_test:
        result = self_test()
        print(json.dumps(result, indent=2))
        return 0 if result["ok"] else 1

    parser.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())