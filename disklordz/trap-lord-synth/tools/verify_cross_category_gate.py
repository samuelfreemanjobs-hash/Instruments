#!/usr/bin/env python3
"""Fail if any cross-category preset probe pair exceeds correlation threshold."""

from __future__ import annotations

import hashlib
import json
import shutil
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
PROBE_DIR = ROOT / "build" / "gate-verify-cross"

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import render_category_probe  # noqa: E402
from trap_lord_audio_metrics import CROSS_CATEGORY_CORR_MAX, category_feature_vector  # noqa: E402


def sync_probe_cache() -> None:
    fp = hashlib.sha256(BANK.read_bytes()).hexdigest()
    meta = PROBE_DIR / ".bank_fingerprint"
    if meta.exists() and meta.read_text(encoding="utf-8").strip() == fp:
        return
    shutil.rmtree(PROBE_DIR, ignore_errors=True)
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    meta.write_text(fp, encoding="utf-8")


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    presets = bank["presets"]
    sync_probe_cache()

    vecs: list[np.ndarray] = []
    cats: list[str] = []
    names: list[str] = []
    for i, p in enumerate(presets):
        wav = PROBE_DIR / f"p_{i:04d}.wav"
        if not wav.exists():
            render_category_probe(p, i, PROBE_DIR)
        vecs.append(category_feature_vector(wav))
        cats.append(p["category"])
        names.append(p.get("name", f"p{i}"))

    failures: list[dict] = []
    worst = 0.0
    worst_pair = (-1, -1)
    for ai in range(len(presets)):
        for bj in range(ai + 1, len(presets)):
            if cats[ai] == cats[bj]:
                continue
            c = float(np.dot(vecs[ai], vecs[bj]))
            if c > worst:
                worst = c
                worst_pair = (ai, bj)
            if c > CROSS_CATEGORY_CORR_MAX:
                failures.append(
                    {
                        "i": ai,
                        "j": bj,
                        "corr": c,
                        "a": {"name": names[ai], "category": cats[ai]},
                        "b": {"name": names[bj], "category": cats[bj]},
                    }
                )

    payload = json.dumps(
        {
            "threshold": CROSS_CATEGORY_CORR_MAX,
            "failures": failures,
            "worst_cross_corr": worst,
            "worst_pair": worst_pair,
        },
        indent=2,
    )
    out = ROOT / "build" / "factory-cross-category-gate.json"
    out.write_text(payload, encoding="utf-8")
    try:
        artifacts_out = Path("/opt/cursor/artifacts/factory-cross-category-gate.json")
        artifacts_out.parent.mkdir(parents=True, exist_ok=True)
        artifacts_out.write_text(payload, encoding="utf-8")
    except OSError as err:
        print(f"WARN: could not write artifacts gate json: {err}")
    print(f"cross-category failures: {len(failures)} worst={worst:.4f} pair={worst_pair}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
