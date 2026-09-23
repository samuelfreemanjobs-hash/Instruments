#!/usr/bin/env python3
"""Run a dry WAV through Osirus FX (default) or another VST3 effect."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from dawdreamer_fx import default_osirus_fx_path, process_wav_through_fx, resolve_fx_plugin


def main() -> int:
    parser = argparse.ArgumentParser(description="Offline WAV → VST3 FX (Osirus FX by default)")
    parser.add_argument("wav_in", type=Path)
    parser.add_argument("--out", type=Path, default=None)
    parser.add_argument("--fx-plugin", type=Path, default=None)
    parser.add_argument("--fx-preset", type=Path, default=None)
    parser.add_argument("--sample-rate", type=int, default=48000)
    args = parser.parse_args()

    fx = resolve_fx_plugin(args.fx_plugin)
    if fx is None:
        fx = default_osirus_fx_path()
    if fx is None or not fx.exists():
        print(
            "FX plugin not found. Build: ./scripts/gearmulator/build-gearmulator-osirus-fx.sh\n"
            "Then: source gearmulator-lane/.gearmulator-osirus-fx.env",
            file=sys.stderr,
        )
        return 1

    out = args.out or args.wav_in.with_name(args.wav_in.stem + "_osirusfx.wav")
    process_wav_through_fx(
        args.wav_in,
        out,
        fx,
        fx_preset=args.fx_preset,
        sample_rate=args.sample_rate,
    )
    print(f"Wrote {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
