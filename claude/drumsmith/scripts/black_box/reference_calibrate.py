#!/usr/bin/env python3
"""Reference calibration — score Black Box WAVs vs elite phonk preview metrics.

Compares punch (front peak), body RMS, duration bands, and peak consistency
against `kits/converter-night/banks/elite_phonk_preview` when present.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from scripts.black_box.curate_hero import score_hit  # noqa: E402
from scripts.black_box.generate_era import kit_masters_dir, load_profile  # noqa: E402


def _collect(folder: Path, limit: int = 80) -> list[dict]:
    rows = []
    if not folder.is_dir():
        return rows
    for wav in sorted(folder.rglob("*.wav")):
        if len(rows) >= limit:
            break
        try:
            rows.append(score_hit(wav))
        except Exception:
            continue
    return rows


def _summary(rows: list[dict]) -> dict:
    if not rows:
        return {"count": 0}
    front = [r["front_peak"] for r in rows]
    body = [r["body_rms"] for r in rows]
    dur = [r["duration"] for r in rows]
    return {
        "count": len(rows),
        "front_peak_median": float(np.median(front)),
        "body_rms_median": float(np.median(body)),
        "duration_median": float(np.median(dur)),
        "peak_consistency": float(1.0 - min(1.0, np.std(front) / (np.mean(front) + 1e-9))),
    }


def calibrate(era: str) -> dict:
    profile = load_profile(era)
    masters = kit_masters_dir(profile)
    ref = ROOT / "kits/converter-night/banks/elite_phonk_preview"
    ours = _collect(masters, limit=120)
    refs = _collect(ref, limit=120)
    ours_s = _summary(ours)
    refs_s = _summary(refs)
    gaps = {}
    for key in ("front_peak_median", "body_rms_median", "duration_median", "peak_consistency"):
        if ours_s.get("count") and refs_s.get("count"):
            a, b = ours_s[key], refs_s[key]
            gaps[key] = {
                "ours": a,
                "ref": b,
                "delta": a - b,
                "ratio": (a / b) if b else None,
            }
    grade = "A"
    notes = []
    if refs_s.get("count", 0) == 0:
        grade = "N/A"
        notes.append("No elite preview reference found — install/build elite_phonk_preview for delta.")
    else:
        fp = gaps.get("front_peak_median", {}).get("ratio") or 1.0
        br = gaps.get("body_rms_median", {}).get("ratio") or 1.0
        if fp < 0.75:
            grade = "C"
            notes.append("Front punch below commercial reference — raise click/drive.")
        elif fp < 0.9:
            grade = "B"
            notes.append("Front punch slightly soft vs reference.")
        if br < 0.7:
            grade = "C" if grade == "A" else grade
            notes.append("Body energy thin — check layered mid bark / membrane gains.")
        if not notes:
            notes.append("Within band of elite preview medians.")
    return {
        "sku": profile["sku"],
        "grade": grade,
        "ours": ours_s,
        "reference": refs_s,
        "gaps": gaps,
        "notes": notes,
        "release_ready": False,
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("era", nargs="?", default="bb96")
    parser.add_argument("--all", action="store_true")
    parser.add_argument("--out", type=Path, default=Path("/opt/cursor/artifacts/Black_Box_Reference_Calibration.json"))
    args = parser.parse_args()
    eras = ("bb93", "bb96", "bb98", "bb2000") if args.all else (args.era,)
    report = {e: calibrate(e) for e in eras}
    args.out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    md = args.out.with_suffix(".md")
    lines = ["# Black Box — reference calibration", ""]
    for e, r in report.items():
        lines.append(f"## {r['sku']} — **{r['grade']}**")
        for n in r["notes"]:
            lines.append(f"- {n}")
        lines.append("")
    md.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({k: v["grade"] for k, v in report.items()}, indent=2))
    print(f"→ {args.out}")


if __name__ == "__main__":
    main()
