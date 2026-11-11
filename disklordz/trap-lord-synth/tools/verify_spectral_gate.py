#!/usr/bin/env python3
"""Verify category-phrase renders pass per-category spectral rules."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
PROBE_DIR = ROOT / "build" / "spectral-gate-verify"

sys.path.insert(0, str(ROOT / "tools"))
from preset_probe import render_category_probe  # noqa: E402
from trap_lord_audio_metrics import analyze_file, category_spectral_passes  # noqa: E402


def main() -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    failures = []
    for i, p in enumerate(bank["presets"]):
        cat = p["category"]
        wav = PROBE_DIR / f"p_{i:04d}.wav"
        if not wav.exists():
            render_category_probe(p, i, PROBE_DIR)
        m = analyze_file(wav, category=cat)
        ok, reason = category_spectral_passes(m, cat)
        if not ok:
            failures.append({"index": i, "name": p.get("name"), "category": cat, "reason": reason, **m.__dict__})

    out = Path("/opt/cursor/artifacts/factory-spectral-gate.json")
    out.write_text(json.dumps({"failures": failures}, indent=2), encoding="utf-8")
    print(f"spectral failures: {len(failures)}")
    for f in failures[:8]:
        print(f"  [{f['index']}] {f['name']}: {f['reason']}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
