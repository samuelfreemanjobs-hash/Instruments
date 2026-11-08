#!/usr/bin/env python3
"""Daily autonomous preset batch — VST Preset Agent (+ workflow-automation cron)."""

from __future__ import annotations

import argparse
import random
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts/vst"))

from vst_lineage import (  # noqa: E402
    DAILY_PRODUCTS,
    PresetRecord,
    VstLineage,
    load_active_seeds,
    save_active_seeds,
)

WAVE_ARCHETYPES = [
    {
        "category": "TRAP",
        "params": {
            "wtPosition": 0.61,
            "filterCutoff": 0.27,
            "subLevel": 0.96,
            "drive": 0.48,
            "ampSustain": 0.94,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "wtPosition": 0.74,
            "filterCutoff": 0.66,
            "ampDecay": 0.08,
            "ampSustain": 0.0,
            "filtEnvAmt": 0.84,
            "glideMs": 36.0,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "wtPosition": 0.82,
            "detuneCents": -12.0,
            "drive": 0.52,
            "crush": 0.22,
            "filterCutoff": 0.44,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "wtPosition": 0.68,
            "crush": 0.4,
            "drive": 0.62,
            "filterDestructive": True,
            "tapeWobble": 0.2,
        },
    },
]

PLUGG_ARCHETYPES = [
    {
        "category": "TRAP",
        "params": {
            "osc1Wave": "saw",
            "osc2Wave": "saw",
            "unisonAmount": 0.72,
            "subLevel": 0.78,
            "filterCutoff": 0.36,
            "drive": 0.38,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "osc2Octave": 1,
            "osc2DetuneCents": 10.0,
            "ampDecay": 0.075,
            "ampSustain": 0.0,
            "filtEnvAmt": 0.8,
            "glideMs": 28.0,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "unisonAmount": 0.9,
            "osc2DetuneCents": -15.0,
            "filterCutoff": 0.4,
            "chorus": 0.42,
        },
    },
    {
        "category": "TRAP",
        "params": {
            "osc1Wave": "square",
            "filterRes": 0.5,
            "crush": 0.2,
            "drive": 0.5,
            "ampDecay": 0.11,
        },
    },
]


def jitter(params: dict, rng: random.Random, scale: float = 0.06) -> dict:
    out = dict(params)
    for key, val in list(out.items()):
        if isinstance(val, bool):
            continue
        if isinstance(val, (int, float)):
            delta = (rng.random() - 0.5) * 2.0 * scale
            if key.endswith("Level") or "Cutoff" in key or "Position" in key or key == "crush" or key == "drive":
                out[key] = float(max(0.0, min(1.0, float(val) + delta)))
            else:
                out[key] = float(val) + delta * 40.0
    return out


def run_batch(batch_size: int, seed: int | None) -> str:
    day = datetime.now(timezone.utc).strftime("%Y%m%d")
    batch_id = f"daily-{day}-utc"
    rng = random.Random(seed if seed is not None else int(day))

    lineage = VstLineage()
    active = load_active_seeds()
    added = 0

    for i in range(batch_size):
        arch = WAVE_ARCHETYPES[i % len(WAVE_ARCHETYPES)]
        params = jitter(arch["params"], rng)
        name = f"Agent Trap {day}-{i:02d}"
        phash = VstLineage.param_hash(params)
        rec = PresetRecord("wave9090", name, arch["category"], phash, __import__("json").dumps(params), "daily_batch", batch_id)
        if lineage.insert(rec):
            active["wave9090"].append({"name": name, "category": arch["category"], "params": params})
            added += 1

    for i in range(batch_size):
        arch = PLUGG_ARCHETYPES[i % len(PLUGG_ARCHETYPES)]
        params = jitter(arch["params"], rng)
        name = f"Agent Trap {day}-{i:02d}"
        phash = VstLineage.param_hash(params)
        rec = PresetRecord("pluggva", name, arch["category"], phash, __import__("json").dumps(params), "daily_batch", batch_id)
        if lineage.insert(rec):
            active["pluggva"].append({"name": name, "category": arch["category"], "params": params})
            added += 1

    lineage.record_batch(batch_id, f"added {added} presets (wave+plugg)")
    lineage.close()

    save_active_seeds(active)
    codegen = REPO / "scripts/vst/codegen_active_seeds.py"
    subprocess.run([sys.executable, str(codegen)], check=True)
    return batch_id


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--seed", type=int, default=None)
    args = parser.parse_args()
    bid = run_batch(args.batch_size, args.seed)
    print(f"batch_complete: {bid}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
