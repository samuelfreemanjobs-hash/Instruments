#!/usr/bin/env python3
"""Daily autonomous preset batch — 8 chart-grade trap slots per synth (quality-gated)."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts/vst"))

from preset_quality import (  # noqa: E402
    MIN_DAILY_SCORE,
    daily_plugg_presets,
    daily_wave_presets,
)
from vst_lineage import PresetRecord, VstLineage, load_active_seeds, save_active_seeds  # noqa: E402


def run_batch(batch_size: int | None, seed: int | None) -> str:
    day = datetime.now(timezone.utc).strftime("%Y%m%d")
    batch_id = f"daily-{day}-utc"
    _ = seed  # reserved for future A/B; goldens are role-locked

    lineage = VstLineage()
    active = load_active_seeds()
    for product in ("wave9090", "pluggva"):
        active[product] = [
            x for x in active.get(product, [])
            if not x.get("name", "").startswith(("Agent Trap", "Agent "))
        ]
    added = 0
    rejected = 0

    wave_slots = daily_wave_presets(day)
    plugg_slots = daily_plugg_presets(day)

    if batch_size is not None:
        wave_slots = wave_slots[:batch_size]
        plugg_slots = plugg_slots[:batch_size]

    new_wave: list[dict] = []
    new_plugg: list[dict] = []

    for preset in wave_slots:
        phash = VstLineage.param_hash(preset.params)
        meta = {
            "role": preset.role,
            "score": round(preset.score, 1),
            "notes": preset.notes,
        }
        rec = PresetRecord(
            "wave9090",
            preset.name,
            preset.category,
            phash,
            json.dumps({**preset.params, "_meta": meta}),
            "daily_batch_gold",
            batch_id,
        )
        if lineage.insert(rec):
            new_wave.append(
                {"name": preset.name, "category": preset.category, "params": preset.params}
            )
            added += 1
            print(f"OK  wave9090 {preset.name} score={preset.score:.1f} role={preset.role}")
        else:
            rejected += 1
            print(f"SKIP wave duplicate {preset.name}")

    for preset in plugg_slots:
        phash = VstLineage.param_hash(preset.params)
        meta = {
            "role": preset.role,
            "score": round(preset.score, 1),
            "notes": preset.notes,
        }
        rec = PresetRecord(
            "pluggva",
            preset.name,
            preset.category,
            phash,
            json.dumps({**preset.params, "_meta": meta}),
            "daily_batch_gold",
            batch_id,
        )
        if lineage.insert(rec):
            new_plugg.append(
                {"name": preset.name, "category": preset.category, "params": preset.params}
            )
            added += 1
            print(f"OK  pluggva {preset.name} score={preset.score:.1f} role={preset.role}")
        else:
            rejected += 1
            print(f"SKIP plugg duplicate {preset.name}")

    if added == 0:
        print(
            f"WARN: no new presets promoted (min_score={MIN_DAILY_SCORE}, rejected={rejected})",
            file=sys.stderr,
        )

    lineage.record_batch(
        batch_id,
        f"gold batch added={added} rejected={rejected} min_score={MIN_DAILY_SCORE}",
    )
    lineage.close()

    active["wave9090"].extend(new_wave)
    active["pluggva"].extend(new_plugg)
    save_active_seeds(active)
    subprocess.run([sys.executable, REPO / "scripts/vst/codegen_active_seeds.py"], check=True)
    return batch_id


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--batch-size",
        type=int,
        default=None,
        help="Optional cap per product (default: all 8 role slots)",
    )
    parser.add_argument("--seed", type=int, default=None)
    args = parser.parse_args()
    bid = run_batch(args.batch_size, args.seed)
    print(f"batch_complete: {bid}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
