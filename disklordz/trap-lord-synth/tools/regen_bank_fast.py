#!/usr/bin/env python3
"""Regenerate factory bank at full category targets (500-preset roadmap)."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from generate_preset_bank import TARGET_QUOTAS, run_generation, write_bank  # noqa: E402

BANK = ROOT / "assets/factory/preset_bank.json"
SEED = 301508

# Process priority categories first so long runs still land core trap timbres.
PRIORITY_FIRST = ("Pads", "Swells", "Synth Keys", "Leads")


def ordered_targets() -> dict[str, int]:
    rest = [c for c in TARGET_QUOTAS if c not in PRIORITY_FIRST]
    return {cat: TARGET_QUOTAS[cat] for cat in (*PRIORITY_FIRST, *sorted(rest))}


def main() -> int:
    targets = ordered_targets()
    resume: list[dict] = []
    if BANK.exists():
        bank = json.loads(BANK.read_text(encoding="utf-8"))
        resume = list(bank.get("presets") or [])
        if resume:
            print(f"Resuming from {len(resume)} presets on disk", flush=True)
    presets, meta = run_generation(SEED, targets, audio_gate=True, resume_presets=resume)
    write_bank(BANK, presets, meta["achievedQuotas"], meta["gate"])
    report = Path("/opt/cursor/artifacts/factory-diversity-gate-report.json")
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(
        json.dumps({"count": len(presets), "mode": "full_targets", "targets": targets, **meta}, indent=2),
        encoding="utf-8",
    )
    print(f"Wrote {len(presets)} presets")
    for cat, n in sorted(meta["achievedQuotas"].items()):
        print(f"  {cat}: {n} (target {targets.get(cat)})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
