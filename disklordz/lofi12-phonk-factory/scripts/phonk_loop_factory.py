#!/usr/bin/env python3
"""Memphis / phonk drum loop factory — unique loops 60–190 BPM."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
PKG_ROOT = SCRIPT_DIR.parent
sys.path.insert(0, str(SCRIPT_DIR))

from lofi12_prepare import (  # noqa: E402
    LOFI_RATE_12K,
    MAX_SEC_12K,
    prepare_for_lofi12,
    write_wav_mono,
)
from phonk_groove import validate_bpm  # noqa: E402
from phonk_loop_fx import LoopFxParams, fx_dict, fx_from_prompt  # noqa: E402
from phonk_loop_render import bars_that_fit_lofi12, render_phonk_loop  # noqa: E402
from phonk_machine_engine import PROFILES, list_engines, resolve_engine_id  # noqa: E402
from phonk_memphis_lanes import list_lane_ids, resolve_memphis_lane  # noqa: E402


def parse_bpm_from_prompt(
    prompt: str, default: float = 140.0, lane_id: str | None = None
) -> float:
    m = re.search(r"\b(\d{2,3})\s*bpm\b", prompt.lower())
    if m:
        return validate_bpm(float(m.group(1)))
    lane = resolve_memphis_lane(prompt, lane_id)
    if lane:
        return validate_bpm(lane.default_bpm)
    if re.search(r"\b(screw|slow|memphis|90s)\b", prompt.lower()):
        return 78.0
    if re.search(r"\b(drift|phonk)\b", prompt.lower()):
        return 148.0
    return validate_bpm(default)


def main() -> None:
    p = argparse.ArgumentParser(description="Memphis phonk drum loop factory")
    p.add_argument("--prompt", default="1990s memphis phonk dirty 808 cowbell")
    p.add_argument("--bpm", type=float, default=0, help="60–190; 0 = infer from prompt")
    p.add_argument("--bars", type=int, default=0, help="1, 2, or 4; 0 = auto")
    p.add_argument("--variation", type=int, default=0)
    p.add_argument("--batch", type=int, default=1, help="Number of unique loops")
    p.add_argument("--out", type=Path, required=True)
    p.add_argument(
        "--lofi12",
        action="store_true",
        help="Also write 12 kHz mono version trimmed to Lofi-12 max length",
    )
    p.add_argument(
        "--lane",
        default=None,
        help=f"Memphis lane override: {', '.join(list_lane_ids())}",
    )
    p.add_argument(
        "--with-vocals",
        action="store_true",
        help="Mix synthetic Memphis vocal texture under drums (formant synth, not a voice clone)",
    )
    engine_help = ", ".join(e[0] for e in list_engines())
    p.add_argument(
        "--engine",
        default=None,
        help=f"Drum machine engine ({engine_help}, classic)",
    )
    p.add_argument("--filter", type=float, default=-1, help="Mix filter 0–1 (-1 = from prompt)")
    p.add_argument("--reverb", type=float, default=-1, help="Reverb send 0–1 (-1 = from prompt)")
    p.add_argument("--tape", type=float, default=-1, help="Tape wobble/noise 0–1")
    p.add_argument("--drive", type=float, default=-1, help="Saturation 0–1")
    args = p.parse_args()

    fx = fx_from_prompt(args.prompt)
    if args.filter >= 0:
        fx.filter_cutoff = min(1.0, max(0.0, args.filter))
    if args.reverb >= 0:
        fx.reverb_send = min(1.0, max(0.0, args.reverb))
    if args.tape >= 0:
        fx.tape = min(1.0, max(0.0, args.tape))
    if args.drive >= 0:
        fx.drive = min(1.0, max(0.0, args.drive))

    lane = resolve_memphis_lane(args.prompt, args.lane)
    engine = resolve_engine_id(args.prompt, args.engine)
    bpm = (
        validate_bpm(args.bpm)
        if args.bpm > 0
        else parse_bpm_from_prompt(args.prompt, lane_id=args.lane)
    )
    args.out.mkdir(parents=True, exist_ok=True)

    index: list[dict] = []
    for i in range(args.batch):
        variation = args.variation + i
        bars = args.bars
        if bars <= 0:
            bars = 4 if bpm <= 95 else 2

        pcm, meta = render_phonk_loop(
            prompt=args.prompt,
            bpm=bpm,
            bars=bars,
            variation=variation,
            lane_id=args.lane,
            with_vocals=args.with_vocals,
            engine_id=args.engine,
            fx=fx,
        )
        lane_tag = meta["memphisLane"] or "memphis"
        stem = f"{lane_tag}_{engine}_{bpm:.0f}bpm_v{variation:02d}"
        wav_path = args.out / f"{stem}.wav"
        write_wav_mono(wav_path, pcm, meta["sampleRate"])
        entry = {"file": wav_path.name, **meta}

        if args.lofi12:
            fit_bars = bars_that_fit_lofi12(bpm, LOFI_RATE_12K, MAX_SEC_12K)
            if fit_bars < bars:
                pcm_lo, meta_lo = render_phonk_loop(
                    prompt=args.prompt,
                    bpm=bpm,
                    bars=fit_bars,
                    variation=variation,
                    lane_id=args.lane,
                    with_vocals=args.with_vocals,
                    engine_id=args.engine,
                    fx=fx,
                )
            else:
                pcm_lo, meta_lo = pcm, meta
            prepared = prepare_for_lofi12(
                pcm_lo, src_rate=44100, dst_rate=LOFI_RATE_12K, grit=0.15
            )
            lo_path = args.out / f"{stem}_lofi12_12k.wav"
            write_wav_mono(lo_path, prepared, LOFI_RATE_12K)
            entry["lofi12File"] = lo_path.name
            entry["lofi12Bars"] = meta_lo["bars"]

        index.append(entry)
        print(f"Wrote {wav_path} ({meta['durationSec']}s, {meta['hitCount']} hits)")

    manifest = {
        "format": "LOFI12_PHONK_LOOP_BATCH",
        "version": 1,
        "prompt": args.prompt,
        "bpm": bpm,
        "memphisLane": lane.lane_id if lane else None,
        "memphisLaneName": lane.display_name if lane else None,
        "drumEngine": engine,
        "drumEngineLabel": PROFILES.get(engine, PROFILES["mr_tape"]).label,
        "fx": fx_dict(fx),
        "loops": index,
    }
    (args.out / "loop_manifest.json").write_text(json.dumps(manifest, indent=2))
    playbook = PKG_ROOT / "docs" / "PHONK_PROGRAMMING_PLAYBOOK.md"
    if playbook.is_file() and args.batch == 1:
        (args.out / "PHONK_PROGRAMMING_PLAYBOOK.md").write_text(playbook.read_text())


if __name__ == "__main__":
    main()
