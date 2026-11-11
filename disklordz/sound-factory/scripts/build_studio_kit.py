#!/usr/bin/env python3
"""Package Phonk Drum Studio (or any local) WAV folder into a Disklordz factory kit."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit_from_samples import PRESET_ARTIST_LANE, build_kit_from_samples  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(
        description="Build a DISKLORDZ_DRUM_KIT_MANIFEST pack from local studio WAVs."
    )
    p.add_argument("--preset", required=True, choices=sorted(PRESET_ARTIST_LANE.keys()))
    p.add_argument("--prompt", required=True, help="Kit prompt / brief for manifest metadata")
    p.add_argument("--samples-dir", type=Path, required=True, help="Folder of WAV/AIF one-shots")
    p.add_argument("--out", type=Path, required=True, help="Output kit directory")
    p.add_argument(
        "--map",
        type=Path,
        default=None,
        help="Optional JSON map of slot name → filename (e.g. kick → my_kick.wav)",
    )
    p.add_argument(
        "--fill-stubs",
        action="store_true",
        help="Use website/public/samples stubs for missing slots",
    )
    p.add_argument(
        "--vae-checkpoint",
        default="",
        help="Optional checkpoint label recorded in manifest factoryParams",
    )
    args = p.parse_args()

    stub_root = None
    if args.fill_stubs:
        stub_root = Path(__file__).resolve().parents[2] / "website" / "public" / "samples"

    try:
        manifest_path = build_kit_from_samples(
            preset_id=args.preset,
            prompt=args.prompt,
            samples_dir=args.samples_dir,
            out_dir=args.out,
            map_path=args.map,
            stub_root=stub_root,
            vae_checkpoint=args.vae_checkpoint or None,
        )
    except (OSError, ValueError, ImportError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(json.dumps({"manifest": str(manifest_path), "out": str(args.out)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
