#!/usr/bin/env python3
"""Add golden presets for one category when probe gates pass (no full-bank cull)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
PROBE_DIR = ROOT / "build" / "gated-restore-probes"
DEFAULT_GOLDEN = "f1e4526e"

sys.path.insert(0, str(ROOT / "tools"))
import numpy as np

from trap_lord_audio_metrics import (  # noqa: E402
    CROSS_CATEGORY_CORR_MAX,
    analyze_file,
    category_feature_vector,
    category_spectral_passes,
    peak_passes_audible_gate,
)


def load_golden(rev: str) -> dict:
    rel = "disklordz/trap-lord-synth/assets/factory/preset_bank.json"
    repo = ROOT.parent.parent
    raw = subprocess.check_output(["git", "-C", str(repo), "show", f"{rev}:{rel}"], text=True)
    return json.loads(raw)


def main() -> int:
    category = sys.argv[1] if len(sys.argv) > 1 else "Pads"
    golden_rev = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_GOLDEN
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    golden = load_golden(golden_rev)
    presets = list(bank.get("presets") or [])
    names = {p.get("name") for p in presets if p.get("category") == category}
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    kept_vecs: list[np.ndarray] = []
    for i, p in enumerate(presets):
        if p.get("category") == category:
            continue
        wav = PROBE_DIR / f"kept_{i:04d}.wav"
        jp = PROBE_DIR / f"kept_{i:04d}.json"
        if not wav.exists():
            jp.write_text(json.dumps(p), encoding="utf-8")
            subprocess.check_call([str(CLI), "category-probe-json", str(jp), str(wav)], cwd=ROOT)
        kept_vecs.append(category_feature_vector(wav))
    added = 0
    skipped: list[tuple[str, str]] = []
    for p in golden.get("presets") or []:
        if p.get("category") != category:
            continue
        name = p.get("name", "")
        if not name or name in names:
            continue
        tag = f"{category}_{added}_{name[:40].replace(' ', '_')}"
        jp = PROBE_DIR / f"{tag}.json"
        wav = PROBE_DIR / f"{tag}.wav"
        jp.write_text(json.dumps(p), encoding="utf-8")
        subprocess.check_call([str(CLI), "category-probe-json", str(jp), str(wav)], cwd=ROOT)
        metrics = analyze_file(wav, category=category)
        if not peak_passes_audible_gate(metrics.peak):
            skipped.append((name, f"probe peak {metrics.peak:.4f}"))
            continue
        ok, reason = category_spectral_passes(metrics, category)
        if not ok:
            skipped.append((name, reason))
            continue
        cand_v = category_feature_vector(wav)
        cross_hit = False
        for ov in kept_vecs:
            if float(np.dot(cand_v, ov)) > CROSS_CATEGORY_CORR_MAX:
                cross_hit = True
                break
        if cross_hit:
            skipped.append((name, f"cross_category_corr>{CROSS_CATEGORY_CORR_MAX}"))
            continue
        presets.append(p)
        kept_vecs.append(cand_v)
        names.add(name)
        added += 1
        print(f"ADD {name}", flush=True)
    out = {**bank, "presets": presets, "count": len(presets)}
    BANK.write_text(json.dumps(out, indent=2), encoding="utf-8")
    report = ROOT / "build" / f"gated-restore-{category.replace('/', '_')}.json"
    report.write_text(json.dumps({"added": added, "skipped": skipped}, indent=2), encoding="utf-8")
    n_cat = sum(1 for p in presets if p.get("category") == category)
    print(f"{category}: {n_cat} in bank (+{added} added, {len(skipped)} skipped)")
    return 0


if __name__ == "__main__":
    sys.exit(main())