#!/usr/bin/env python3
"""Re-merge factory presets from a golden commit for priority categories (by preset name)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
PRIORITY = ("Synth Keys", "Leads", "Swells")
DEFAULT_GOLDEN = "f1e4526e"


def load_bank_from_git(rev: str) -> dict:
    rel = "disklordz/trap-lord-synth/assets/factory/preset_bank.json"
    repo = ROOT.parent.parent
    raw = subprocess.check_output(["git", "-C", str(repo), "show", f"{rev}:{rel}"], text=True)
    return json.loads(raw)


def main() -> int:
    golden_rev = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_GOLDEN
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    golden = load_bank_from_git(golden_rev)
    presets = list(bank.get("presets") or [])
    target = bank.get("targetQuotas") or bank.get("categoryQuotas") or {}

    names_by_cat: dict[str, set[str]] = {c: set() for c in PRIORITY}
    for p in presets:
        if p.get("category") in names_by_cat:
            names_by_cat[p["category"]].add(p.get("name", ""))

    added = 0
    for p in golden.get("presets") or []:
        cat = p.get("category")
        if cat not in names_by_cat:
            continue
        name = p.get("name", "")
        if name and name not in names_by_cat[cat]:
            presets.append(p)
            names_by_cat[cat].add(name)
            added += 1

    out = {**bank, "presets": presets, "count": len(presets)}
    if "targetQuotas" not in out and target:
        out["targetQuotas"] = target
    BANK.write_text(json.dumps(out, indent=2), encoding="utf-8")
    for cat in PRIORITY:
        n = sum(1 for p in presets if p.get("category") == cat)
        print(f"{cat}: {n} (after merge +{added} total)")
    print(f"Bank total: {len(presets)} (+{added} restored)")
    return 0


if __name__ == "__main__":
    sys.exit(main())