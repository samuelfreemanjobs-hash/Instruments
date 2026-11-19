#!/usr/bin/env python3
"""List golden-vs-bank priority presets not kept and why (audible / spectral gates)."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
PROBE_DIR = ROOT / "build" / "priority-removal-audit"
AUDIBLE_DIR = ROOT / "build" / "priority-removal-audible"
PRIORITY = ("Pads", "Synth Keys", "Leads", "Swells")
DEFAULT_GOLDEN = "f1e4526e"

sys.path.insert(0, str(ROOT / "tools"))
from trap_lord_audio_metrics import (  # noqa: E402
    analyze_file,
    category_spectral_passes,
    peak_passes_audible_gate,
    preset_audible_at_notes,
)


def load_golden(rev: str) -> dict:
    rel = "disklordz/trap-lord-synth/assets/factory/preset_bank.json"
    repo = ROOT.parent.parent
    raw = subprocess.check_output(["git", "-C", str(repo), "show", f"{rev}:{rel}"], text=True)
    return json.loads(raw)


def probe_preset(preset: dict, tag: str) -> Path:
    PROBE_DIR.mkdir(parents=True, exist_ok=True)
    json_path = PROBE_DIR / f"{tag}.json"
    wav = PROBE_DIR / f"{tag}.wav"
    json_path.write_text(json.dumps(preset), encoding="utf-8")
    if not wav.exists():
        subprocess.check_call(
            [str(CLI), "category-probe-json", str(json_path), str(wav)],
            cwd=ROOT,
        )
    return wav


def gate_reason(preset: dict, wav: Path) -> tuple[bool, str]:
    cat = preset.get("category", "")
    metrics = analyze_file(wav, category=cat)
    if not peak_passes_audible_gate(metrics.peak):
        return False, f"probe_peak_below_gate linear={metrics.peak:.4f}"
    ok_sp, reason_sp = category_spectral_passes(metrics, cat)
    if not ok_sp:
        return False, f"spectral: {reason_sp}"
    # Need bank index for audible-at-notes; use json tag path as pseudo via temp bank file is heavy — skip multi-note unless in bank
    return True, "passes_probe_gates"


def main() -> int:
    golden_rev = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_GOLDEN
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    golden = load_golden(golden_rev)
    current_names: dict[str, set[str]] = {c: set() for c in PRIORITY}
    for p in bank.get("presets") or []:
        cat = p.get("category")
        if cat in current_names:
            current_names[cat].add(p.get("name", ""))

    report: list[dict] = []
    cull_logged = ROOT / "build" / "priority-cull-removals.json"
    cull_by_name: dict[str, str] = {}
    if cull_logged.exists():
        for row in json.loads(cull_logged.read_text(encoding="utf-8")):
            cull_by_name[row.get("name", "")] = row.get("reason", "")

    for cat in PRIORITY:
        golden_cat = [p for p in golden.get("presets") or [] if p.get("category") == cat]
        for p in golden_cat:
            name = p.get("name", "")
            if not name or name in current_names[cat]:
                continue
            tag = name.replace(" ", "_")[:80]
            wav = probe_preset(p, tag)
            passes, reason = gate_reason(p, wav)
            if passes and name in cull_by_name:
                reason = cull_by_name[name]
            elif passes:
                reason = "missing_after_cull (not in bank; passes probe gates)"
            report.append(
                {
                    "category": cat,
                    "name": name,
                    "passes_probe_gates": passes,
                    "reason": reason,
                }
            )

    out = ROOT / "build" / "priority-removal-report.json"
    out.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(f"Wrote {out} ({len(report)} golden priority presets not in bank)")
    for row in report:
        print(f"  [{row['category']}] {row['name']}: {row['reason']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())