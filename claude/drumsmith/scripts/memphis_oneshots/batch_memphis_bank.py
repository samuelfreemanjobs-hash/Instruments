#!/usr/bin/env python3
"""Batch MEMPHIS-660 kick/snare/hat oneshots into a Converter Night kit bank folder.

Writes PCM16 mono @ 44.1 kHz (kit delivery). WAV output is gitignored; commit
bank-recipe.json + README only. Original synthesis — no copyrighted PCM.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat  # noqa: E402
from scripts.memphis_oneshots.phonk_kick import (  # noqa: E402
    NOTE_HZ as KICK_NOTE_HZ,
    generate_memphis_phonk_kick,
    write_wav_16bit_mono,
)
from scripts.memphis_oneshots.phonk_snare import (  # noqa: E402
    NOTE_HZ as SNARE_NOTE_HZ,
    generate_memphis_phonk_snare,
)

DEFAULT_OUT = ROOT / "kits" / "converter-night" / "banks" / "memphis660"
RECIPE_PATH = Path(__file__).resolve().parent / "bank_recipe.json"


def _load_recipe(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def render_bank(out_dir: Path, recipe: dict) -> list[dict]:
    """Render all recipe entries; return manifest rows."""
    kicks_dir = out_dir / "Kicks"
    snares_dir = out_dir / "Snares"
    hats_dir = out_dir / "Hi-Hats_Closed"
    rows: list[dict] = []

    for i, entry in enumerate(recipe.get("kicks", []), start=1):
        key = entry.get("key", "F1")
        freq = float(entry.get("freq") or KICK_NOTE_HZ.get(key, 43.65))
        drive = float(entry.get("drive_db", 4.0))
        name = f"CNV_01_KICK_MemphisPhonk_{key.replace('#', 's')}_{i:02d}.wav"
        path = kicks_dir / name
        buf = generate_memphis_phonk_kick(
            root_freq=freq,
            clip_drive_db=drive,
            pitch_mod_depth=float(entry.get("pitch_st", 48.0)),
        )
        write_wav_16bit_mono(path, buf)
        rows.append({"kind": "KICK", "path": str(path.relative_to(out_dir)), "key": key, "freq": freq})

    for i, entry in enumerate(recipe.get("snares", []), start=1):
        key = entry.get("key", "A3")
        freq = float(entry.get("freq") or SNARE_NOTE_HZ.get(key, 220.0))
        name = f"CNV_01_SNR_MemphisPhonk_{key.replace('#', 's')}_{i:02d}.wav"
        path = snares_dir / name
        buf = generate_memphis_phonk_snare(
            membrane_root=freq,
            clip_drive_db=float(entry.get("drive_db", 5.5)),
            flam_delay_ms=float(entry.get("flam_ms", 4.0)),
            seed=int(entry.get("seed", 660 + i)),
        )
        write_wav_16bit_mono(path, buf)
        rows.append({"kind": "SNARE", "path": str(path.relative_to(out_dir)), "key": key, "freq": freq})

    for i, entry in enumerate(recipe.get("hats", []), start=1):
        center = float(entry.get("center_hz", 9000.0))
        name = f"CNV_01_CH_MemphisEdge_{int(center)}_{i:02d}.wav"
        path = hats_dir / name
        buf = generate_memphis_phonk_hat(
            center_hz=center,
            seed=int(entry.get("seed", 808 + i)),
            noise_decay=float(entry.get("decay", 0.028)),
        )
        write_wav_16bit_mono(path, buf)
        rows.append({"kind": "HAT", "path": str(path.relative_to(out_dir)), "center_hz": center})

    manifest = {
        "sku": "CNV",
        "bank": "memphis660",
        "generator": "memphis_oneshots",
        "release_ready": False,
        "delivery": {"sample_rate": 44100, "bit_depth": 16, "channels": 1},
        "samples": rows,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return rows


def main() -> None:
    parser = argparse.ArgumentParser(description="Batch Memphis phonk oneshots into a kit bank")
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT)
    parser.add_argument("--recipe", type=Path, default=RECIPE_PATH)
    args = parser.parse_args()

    recipe = _load_recipe(args.recipe)
    rows = render_bank(args.out, recipe)
    print(f"wrote {len(rows)} samples → {args.out}")
    for kind in ("KICK", "SNARE", "HAT"):
        n = sum(1 for r in rows if r["kind"] == kind)
        print(f"  {kind}: {n}")


if __name__ == "__main__":
    main()
