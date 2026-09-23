#!/usr/bin/env python3
"""Render one catalog instrument to multisample WAVs + instrument-map.json."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from emit_sfz import emit_sfz
from factory_qa import analyze_wav
from trap_synth import LANE_PATCH, ProducerLane, default_multisample_roots, render_note
from wav_util import SAMPLE_RATE, write_mono_wav

DURATION_S = 2.8


def build_zones(
    out_dir: Path,
    instrument_id: str,
    lane: ProducerLane,
    seed: int,
    roots: list[int] | None = None,
) -> dict:
    patch = LANE_PATCH.get(lane, LANE_PATCH["neutral"])
    roots = roots or default_multisample_roots()
    samples_dir = out_dir / "samples"
    zones = []
    for root in roots:
        mid = (root - 1, root + 1)
        lo, hi = max(0, mid[0]), min(127, mid[1])
        fname = f"{instrument_id}_root{root}.wav"
        rel = f"samples/{fname}"
        path = out_dir / rel
        buf = render_note(root, 100, DURATION_S, patch, seed=seed + root)
        sha = write_mono_wav(path, buf)
        qa = analyze_wav(path)
        if not qa.pass_qa:
            raise RuntimeError(f"QA failed for {path}: {qa.reasons}")
        zones.append(
            {
                "samplePath": rel,
                "rootKey": root,
                "loKey": lo,
                "hiKey": hi,
                "loVel": 1,
                "hiVel": 127,
                "volumeDb": 0.0,
            }
        )
    return {
        "format": "DISKLORDZ_INSTRUMENT_MAP",
        "version": 1,
        "instrumentId": instrument_id,
        "name": instrument_id,
        "sampleRate": SAMPLE_RATE,
        "zones": zones,
    }


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--instrument-id", required=True)
    p.add_argument("--lane", default="jeezy", choices=list(LANE_PATCH.keys()))
    p.add_argument("--seed", type=int, default=0)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--emit-sfz", action="store_true")
    args = p.parse_args()

    if args.seed == 0:
        args.seed = int(hashlib.sha256(args.instrument_id.encode()).hexdigest()[:8], 16)

    args.out.mkdir(parents=True, exist_ok=True)
    imap = build_zones(args.out, args.instrument_id, args.lane, args.seed)
    map_path = args.out / "instrument-map.json"
    map_path.write_text(json.dumps(imap, indent=2), encoding="utf-8")
    sfz = None
    if args.emit_sfz:
        sfz = str(emit_sfz(imap, args.out))
    print(json.dumps({"map": str(map_path), "sfz": sfz, "zones": len(imap["zones"])}, indent=2))


if __name__ == "__main__":
    main()
