#!/usr/bin/env python3
"""Curate a deadly hero bank from Black Box masters (punch + energy scoring)."""

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

from scripts.black_box.generate_era import kit_masters_dir, load_profile  # noqa: E402
from scripts.black_box.process_fx import read_wav_mono  # noqa: E402


def score_hit(path: Path) -> dict:
    x = read_wav_mono(path)
    sr = 44100
    n = len(x)
    front = x[: min(n, int(0.012 * sr))]
    body = x[int(0.012 * sr) : max(int(0.012 * sr) + 1, int(0.2 * sr))]
    tail = x[max(0, n - int(0.15 * sr)) :]
    peak = float(np.max(np.abs(x))) or 1e-9
    front_peak = float(np.max(np.abs(front))) if len(front) else 0.0
    body_rms = float(np.sqrt(np.mean(body**2))) if len(body) else 0.0
    tail_rms = float(np.sqrt(np.mean(tail**2))) if len(tail) else 0.0
    # Prefer hard attack + musical body; 808s keep tail energy
    kind = path.name.split("_")[1] if "_" in path.name else ""
    score = 0.45 * (front_peak / peak) + 0.35 * body_rms + 0.20 * (tail_rms if kind == "808" else front_peak)
    return {
        "path": str(path),
        "name": path.name,
        "kind": kind,
        "score": score,
        "front_peak": front_peak,
        "body_rms": body_rms,
        "tail_rms": tail_rms,
        "duration": n / sr,
    }


def select_heroes(masters: Path, *, per_kind: int = 8, max_total: int = 96) -> list[Path]:
    by_kind: dict[str, list[tuple[float, Path]]] = {}
    for wav in sorted(masters.rglob("*.wav")):
        if "loop" in wav.parent.name.lower():
            continue
        try:
            s = score_hit(wav)
        except Exception:
            continue
        by_kind.setdefault(s["kind"] or "X", []).append((s["score"], wav))
    picked: list[Path] = []
    for kind, rows in sorted(by_kind.items()):
        rows.sort(key=lambda t: t[0], reverse=True)
        for _, path in rows[:per_kind]:
            picked.append(path)
            if len(picked) >= max_total:
                return picked
    return picked


def build_hero_manifest(heroes: list[Path], profile: dict) -> dict:
    rows = []
    for path in heroes:
        try:
            rows.append(score_hit(path))
        except Exception:
            rows.append({"name": path.name, "path": str(path), "score": 0.0})
    rows.sort(key=lambda r: r.get("score", 0.0), reverse=True)
    return {
        "sku": profile.get("sku"),
        "title": profile.get("title"),
        "count": len(rows),
        "note": "Curated hero bank — release_ready still false until HITL",
        "heroes": rows,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Curate Black Box hero bank")
    parser.add_argument("era", help="bb93|bb96|bb98|bb2000")
    parser.add_argument("--out", type=Path, default=None)
    parser.add_argument("--per-kind", type=int, default=8)
    parser.add_argument("--max-total", type=int, default=96)
    args = parser.parse_args()
    profile = load_profile(args.era)
    masters = kit_masters_dir(profile)
    heroes = select_heroes(masters, per_kind=args.per_kind, max_total=args.max_total)
    manifest = build_hero_manifest(heroes, profile)
    out = args.out or Path("/opt/cursor/artifacts") / f"{profile['sku']}_Hero_Manifest.json"
    out.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"{profile['sku']} heroes={len(heroes)} → {out}")


if __name__ == "__main__":
    main()
