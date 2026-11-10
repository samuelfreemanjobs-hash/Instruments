#!/usr/bin/env python3
"""Memphis/phonk vocal texture factory (synthetic chops + optional acapella chop)."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))

from lofi12_prepare import LOFI_RATE_24K, prepare_for_lofi12, read_wav_mono, write_wav_mono  # noqa: E402
from phonk_groove import validate_bpm  # noqa: E402
from phonk_loop_factory import parse_bpm_from_prompt  # noqa: E402
from phonk_memphis_lanes import list_lane_ids, resolve_memphis_lane  # noqa: E402
from phonk_vocal_synth import (  # noqa: E402
    phonkify,
    render_vocal_chop_pack,
    render_vocal_loop,
    resolve_vocal_params,
)

SRC_RATE = 44100


def chop_user_acapella(path: Path, params_prompt: str, variation: int) -> dict[str, list[float]]:
    """Pitch-down + slice user-provided mono WAV into phonk-style stabs."""
    pcm, rate = read_wav_mono(path)
    lane = resolve_memphis_lane(params_prompt, None)
    vp = resolve_vocal_params(params_prompt, variation, lane)
    if rate != SRC_RATE:
        pcm = prepare_for_lofi12(pcm, src_rate=rate, dst_rate=SRC_RATE, grit=0)

    pcm = phonkify(pcm, vp)
    slice_len = int(44100 * 0.35)
    out: dict[str, list[float]] = {}
    for i in range(4):
        start = (variation * 17 + i * 31) * 997 % max(1, len(pcm) - slice_len)
        out[f"acapella_chop_{i + 1}"] = pcm[start : start + slice_len]
    return out


def main() -> None:
    p = argparse.ArgumentParser(description="Memphis phonk vocal texture factory")
    p.add_argument("--prompt", default="memphis phonk screw dark vocal chop")
    p.add_argument("--bpm", type=float, default=0)
    p.add_argument("--bars", type=int, default=2)
    p.add_argument("--variation", type=int, default=0)
    p.add_argument("--batch", type=int, default=1)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--lane", default=None, help=f"Optional: {', '.join(list_lane_ids())}")
    p.add_argument(
        "--chops-only",
        action="store_true",
        help="Export stab pack (uh/ah/oh/ay) instead of full loop",
    )
    p.add_argument(
        "--acapella",
        type=Path,
        default=None,
        help="Your mono WAV — sliced and phonk-processed (you must own rights)",
    )
    p.add_argument("--lofi12", action="store_true", help="Also write 24 kHz mono versions")
    args = p.parse_args()

    lane = resolve_memphis_lane(args.prompt, args.lane)
    bpm = validate_bpm(args.bpm) if args.bpm > 0 else parse_bpm_from_prompt(args.prompt, args.lane)
    args.out.mkdir(parents=True, exist_ok=True)

    entries: list[dict] = []
    for i in range(args.batch):
        var = args.variation + i
        if args.acapella:
            pack = chop_user_acapella(args.acapella, args.prompt, var)
            for name, pcm in pack.items():
                path = args.out / f"{name}_v{var:02d}.wav"
                write_wav_mono(path, pcm, SRC_RATE)
                entries.append({"file": path.name, "kind": "acapella_chop", "variation": var})
            continue

        if args.chops_only:
            pack = render_vocal_chop_pack(args.prompt, var, lane)
            for name, pcm in pack.items():
                path = args.out / f"{name}_v{var:02d}.wav"
                write_wav_mono(path, pcm, SRC_RATE)
                if args.lofi12:
                    lo = prepare_for_lofi12(pcm, src_rate=SRC_RATE, dst_rate=LOFI_RATE_24K, grit=0.1)
                    lo_path = args.out / f"{name}_v{var:02d}_lofi24k.wav"
                    write_wav_mono(lo_path, lo, LOFI_RATE_24K)
                entries.append({"file": path.name, "kind": "synthetic_chop", "variation": var})
        else:
            pcm, meta = render_vocal_loop(
                prompt=args.prompt,
                bpm=bpm,
                bars=args.bars,
                variation=var,
                lane=lane,
            )
            lane_tag = meta.get("memphisLane") or "vocal"
            path = args.out / f"{lane_tag}_vocal_{bpm:.0f}bpm_v{var:02d}.wav"
            write_wav_mono(path, pcm, SRC_RATE)
            entry = {"file": path.name, **meta}
            if args.lofi12:
                lo = prepare_for_lofi12(pcm, src_rate=SRC_RATE, dst_rate=LOFI_RATE_24K, grit=0.1)
                lo_path = args.out / f"{lane_tag}_vocal_{bpm:.0f}bpm_v{var:02d}_lofi24k.wav"
                write_wav_mono(lo_path, lo, LOFI_RATE_24K)
                entry["lofi12File"] = lo_path.name
            entries.append(entry)
            print(f"Wrote {path} ({meta['durationSec']}s synthetic vocal loop)")

    manifest = {
        "format": "LOFI12_PHONK_VOCAL_BATCH",
        "version": 1,
        "prompt": args.prompt,
        "bpm": bpm,
        "synthetic": args.acapella is None,
        "memphisLane": lane.lane_id if lane else None,
        "entries": entries,
        "legalNote": "Synthetic output is formant texture only. For commercial phonk, use licensed or original acapellas via --acapella.",
    }
    (args.out / "vocal_manifest.json").write_text(json.dumps(manifest, indent=2))


if __name__ == "__main__":
    main()
