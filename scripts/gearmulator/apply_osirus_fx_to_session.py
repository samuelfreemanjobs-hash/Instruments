#!/usr/bin/env python3
"""Apply Osirus FX to every zone WAV in a capture session; refresh instrument.map.json."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from dawdreamer_fx import default_osirus_fx_path, process_wav_through_fx, resolve_fx_plugin
from map_lib import finalize_session_dir


def main() -> int:
    parser = argparse.ArgumentParser(description="Post-process session WAVs through Osirus FX")
    parser.add_argument("session_dir", type=Path)
    parser.add_argument("--fx-plugin", type=Path, default=None)
    parser.add_argument("--fx-preset", type=Path, default=None)
    parser.add_argument("--in-place", action="store_true", help="Overwrite original WAVs")
    args = parser.parse_args()

    session = args.session_dir.resolve()
    manifest = session / "manifest.tsv"
    if not manifest.is_file():
        print(f"Missing {manifest}", file=sys.stderr)
        return 1

    fx = resolve_fx_plugin(args.fx_plugin) or default_osirus_fx_path()
    if fx is None or not fx.exists():
        print("Osirus FX VST3 not found.", file=sys.stderr)
        return 1

    for wav in sorted(session.glob("note_*.wav")):
        target = wav if args.in_place else wav.with_name(wav.stem + "_fx.wav")
        process_wav_through_fx(wav, target, fx, fx_preset=args.fx_preset)
        if not args.in_place:
            print(f"Wrote {target}")

    # Patch manifest fx columns if empty
    lines = manifest.read_text(encoding="utf-8").splitlines()
    if len(lines) < 2:
        return 0
    header = lines[0].split("\t")
    try:
        fx_i = header.index("fx_plugin_path")
        preset_i = header.index("fx_preset_path")
    except ValueError:
        print("Manifest missing fx columns; re-run capture with DawDreamer.", file=sys.stderr)
        return 1

    fx_str = str(fx.resolve())
    preset_str = str(args.fx_preset) if args.fx_preset else ""
    new_lines = [lines[0]]
    for row in lines[1:]:
        cols = row.split("\t")
        while len(cols) < len(header):
            cols.append("")
        cols[fx_i] = fx_str
        cols[preset_i] = preset_str
        if not args.in_place and "_fx.wav" not in cols[-1]:
            wav_path = Path(cols[-1])
            cols[-1] = str(session / (wav_path.stem + "_fx.wav"))
        new_lines.append("\t".join(cols))
    manifest.write_text("\n".join(new_lines) + "\n", encoding="utf-8")

    finalize_session_dir(session)
    print(f"Updated {session / 'instrument.map.json'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
