#!/usr/bin/env python3
"""Batch-render a note grid via DawDreamer (VST3/VST host in Python).

Requires: pip install -r gearmulator-lane/requirements-dawdreamer.txt

Example:
  python3 scripts/gearmulator/render_multisample_dawdreamer.py \\
    --plugin build/Wave909/Wave909_artefacts/Release/VST3/WAVE-909.vst3 \\
    --name wave909_smoke --note-start 60 --note-end 63 --note-step 1
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import dawdreamer as daw
import numpy as np
from scipy.io import wavfile


def normalise_vst3_path(path: Path) -> Path:
    p = path.resolve()
    if p.suffix == ".vst3" and p.is_file():
        return p.parent
    return p


def render_note(
    plugin: Path,
    out_wav: Path,
    *,
    program: int,
    note: int,
    velocity: int,
    seconds: float,
    sample_rate: int,
    block_size: int,
) -> None:
    engine = daw.RenderEngine(sample_rate, block_size)
    synth = engine.make_plugin_processor("synth", str(normalise_vst3_path(plugin)))
    engine.load_graph([(synth, [])])

    if program >= 0:
        try:
            synth.set_program(program)
        except AttributeError:
            pass

    synth.add_midi_note(note, velocity, 0.0, seconds)
    engine.render(seconds)
    audio = engine.get_audio()
    if audio.size == 0:
        raise RuntimeError(f"Empty render for note {note}")

    out_wav.parent.mkdir(parents=True, exist_ok=True)
    wavfile.write(str(out_wav), sample_rate, audio.T)


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description="DawDreamer multisample grid")
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, default=repo_root / "gearmulator-lane/multisamples/out")
    parser.add_argument("--name", default="dawdreamer_capture")
    parser.add_argument("--program", type=int, default=0)
    parser.add_argument("--note-start", type=int, default=36)
    parser.add_argument("--note-end", type=int, default=84)
    parser.add_argument("--note-step", type=int, default=3)
    parser.add_argument("--velocity", type=int, default=100)
    parser.add_argument("--seconds", type=float, default=3.0)
    parser.add_argument("--sample-rate", type=int, default=48000)
    parser.add_argument("--block-size", type=int, default=512)
    args = parser.parse_args()

    if not args.plugin.exists():
        print(f"Plugin not found: {args.plugin}", file=sys.stderr)
        return 1

    session = f"{args.name}_p{args.program}_v{args.velocity}"
    dest = args.out_dir / session
    dest.mkdir(parents=True, exist_ok=True)
    manifest = dest / "manifest.tsv"
    manifest.write_text("program\tnote\tvelocity\tseconds\tsample_rate\tengine\twav\n", encoding="utf-8")

    for note in range(args.note_start, args.note_end + 1, args.note_step):
        wav_path = dest / f"note_{note:03d}_vel_{args.velocity:03d}.wav"
        print(f"Rendering note {note} -> {wav_path}")
        render_note(
            args.plugin,
            wav_path,
            program=args.program,
            note=note,
            velocity=args.velocity,
            seconds=args.seconds,
            sample_rate=args.sample_rate,
            block_size=args.block_size,
        )
        with manifest.open("a", encoding="utf-8") as mf:
            mf.write(
                f"{args.program}\t{note}\t{args.velocity}\t{args.seconds}\t"
                f"{args.sample_rate}\tdawdreamer\t{wav_path}\n"
            )

    print(f"Done. Manifest: {manifest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
