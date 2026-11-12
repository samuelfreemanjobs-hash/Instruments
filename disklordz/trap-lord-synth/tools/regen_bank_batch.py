#!/usr/bin/env python3
"""Regenerate selected factory categories (resume others), then optional pipeline hook."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from generate_preset_bank import TARGET_QUOTAS, run_generation, write_bank  # noqa: E402

BANK = ROOT / "assets/factory/preset_bank.json"
DEFAULT_SEED = 301508

PRIORITY_BATCHES = {
    "priority-core": ("Pads", "Swells", "Synth Keys", "Leads"),
    "rest": tuple(c for c in TARGET_QUOTAS if c not in {"Pads", "Swells", "Synth Keys", "Leads"}),
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Regenerate one or more Trap Lord factory categories.")
    parser.add_argument(
        "--categories",
        type=str,
        default="",
        help="Comma-separated categories (default: all in --batch)",
    )
    parser.add_argument(
        "--batch",
        choices=sorted(PRIORITY_BATCHES.keys()),
        default="",
        help="Named batch (priority-core = Pads/Swells/Synth Keys/Leads)",
    )
    parser.add_argument("--seed", type=int, default=DEFAULT_SEED)
    parser.add_argument(
        "--pipeline",
        action="store_true",
        help="Run tools/post_regen_pipeline.sh after generation",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.batch:
        cats = list(PRIORITY_BATCHES[args.batch])
    elif args.categories:
        cats = [c.strip() for c in args.categories.split(",") if c.strip()]
    else:
        raise SystemExit("Specify --categories or --batch")

    for c in cats:
        if c not in TARGET_QUOTAS:
            raise SystemExit(f"Unknown category: {c}")

    resume: list[dict] = []
    if BANK.exists():
        bank = json.loads(BANK.read_text(encoding="utf-8"))
        resume = list(bank.get("presets") or [])
        print(f"Resume bank: {len(resume)} presets", flush=True)

    target = dict(TARGET_QUOTAS)
    regen_only = set(cats)
    presets, meta = run_generation(
        args.seed,
        target,
        audio_gate=True,
        resume_presets=resume,
        regen_only=regen_only,
    )
    write_bank(BANK, presets, meta["achievedQuotas"], meta["gate"])
    report = ROOT / "build" / f"factory-regen-{'-'.join(c.replace('/', '_') for c in cats)}.json"
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(
        json.dumps(
            {"categories": cats, "count": len(presets), "achievedQuotas": meta["achievedQuotas"]},
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Bank total: {len(presets)} presets")
    for cat in cats:
        print(f"  {cat}: {meta['achievedQuotas'].get(cat)} (target {TARGET_QUOTAS[cat]})")

    if args.pipeline:
        subprocess.check_call(["bash", str(ROOT / "tools/post_regen_pipeline.sh")], cwd=ROOT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
