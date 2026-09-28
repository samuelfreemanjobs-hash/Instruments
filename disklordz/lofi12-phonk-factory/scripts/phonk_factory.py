#!/usr/bin/env python3
"""Generate a Lofi-12–ready phonk drum bank (24 kHz mono WAV + load manifest)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import asdict
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
PKG_ROOT = SCRIPT_DIR.parent
sys.path.insert(0, str(SCRIPT_DIR))

from bank_layout import PHONK_BANK_A, filename_for_slot  # noqa: E402
from lofi12_prepare import (  # noqa: E402
    LOFI_RATE_24K,
    prepare_for_lofi12,
    read_wav_mono,
    write_wav_mono,
)
from phonk_machine_engine import PROFILES, list_engines, resolve_engine_id  # noqa: E402
from phonk_style_presets import get_preset, list_preset_ids  # noqa: E402
from phonk_synth import resolve_phonk_params, render_slot  # noqa: E402

DEFAULT_BANK_PROMPT = "dirty memphis phonk 808 cowbell 140"

DISKLORDZ_MAP = {
    "kick": "kick_808",
    "snare": "snare_memphis",
    "hat_closed": "hat_closed",
    "hat_open": "hat_open",
    "rim": "snare_rim",
    "clap": "clap",
}


def _write_slot_wav(
    out_dir: Path,
    slot_key: str,
    samples_44k: list[float],
    *,
    grit: float,
    dst_rate: int,
) -> dict:
    prepared = prepare_for_lofi12(
        samples_44k, src_rate=44100, dst_rate=dst_rate, grit=grit
    )
    spec = next(s for s in PHONK_BANK_A if s.key == slot_key)
    fname = filename_for_slot(spec)
    path = out_dir / fname
    write_wav_mono(path, prepared, dst_rate)
    return {
        "slot": spec.index,
        "key": slot_key,
        "filename": fname,
        "samples": len(prepared),
        "sampleRate": dst_rate,
        "durationSec": round(len(prepared) / dst_rate, 4),
    }


def generate_bank(
    *,
    prompt: str,
    out_dir: Path,
    variation: int = 0,
    dst_rate: int = LOFI_RATE_24K,
    engine_id: str | None = None,
) -> dict:
    engine = resolve_engine_id(prompt, engine_id)
    params = resolve_phonk_params(prompt, variation)
    out_dir.mkdir(parents=True, exist_ok=True)
    entries: list[dict] = []
    for spec in PHONK_BANK_A:
        raw = render_slot(spec.key, params, engine)
        entries.append(
            _write_slot_wav(out_dir, spec.key, raw, grit=params.grit, dst_rate=dst_rate)
        )

    guide_src = PKG_ROOT / "docs" / "PATTERN_GUIDE.md"
    if guide_src.is_file():
        (out_dir / "PATTERN_GUIDE.md").write_text(guide_src.read_text())

    manifest = {
        "format": "LOFI12_PHONK_BANK",
        "version": 1,
        "prompt": prompt,
        "variation": variation,
        "params": {k: (round(v, 4) if isinstance(v, float) else v) for k, v in asdict(params).items()},
        "drumEngine": engine,
        "drumEngineLabel": PROFILES.get(engine, PROFILES["mr_tape"]).label,
        "device": {
            "model": "LIVEN Lofi-12",
            "targetBank": "A",
            "sampleRateHz": dst_rate,
            "note": "Sample from LINE IN or import via SysEx when encoder available",
        },
        "slots": entries,
    }
    (out_dir / "load_manifest.json").write_text(json.dumps(manifest, indent=2))
    return manifest


def import_disklordz_kit(
    kit_dir: Path, out_dir: Path, *, prompt: str, dst_rate: int, engine_id: str | None = None
) -> dict:
    manifest_path = kit_dir / "manifest.json"
    if not manifest_path.is_file():
        raise SystemExit(f"Missing manifest.json in {kit_dir}")

    kit_manifest = json.loads(manifest_path.read_text())
    engine = resolve_engine_id(prompt or kit_manifest.get("prompt", "phonk"), engine_id)
    params = resolve_phonk_params(prompt or kit_manifest.get("prompt", "phonk"))
    imported: dict[str, list[float]] = {}

    for sample in kit_manifest.get("samples", []):
        name = sample.get("name")
        fname = sample.get("filename")
        if not name or not fname:
            continue
        slot_key = DISKLORDZ_MAP.get(name)
        if not slot_key:
            continue
        wav_path = kit_dir / fname
        if wav_path.is_file():
            pcm, rate = read_wav_mono(wav_path)
            if rate != 44100:
                pcm = prepare_for_lofi12(pcm, src_rate=rate, dst_rate=44100, grit=0)
            imported[slot_key] = pcm

    out_dir.mkdir(parents=True, exist_ok=True)
    entries: list[dict] = []
    for spec in PHONK_BANK_A:
        if spec.key in imported:
            raw = imported[spec.key]
        else:
            raw = render_slot(spec.key, params, engine)
        entries.append(
            _write_slot_wav(out_dir, spec.key, raw, grit=params.grit, dst_rate=dst_rate)
        )

    manifest = {
        "format": "LOFI12_PHONK_BANK",
        "version": 1,
        "prompt": prompt or kit_manifest.get("prompt", ""),
        "sourceKit": str(kit_dir),
        "disklordzPreset": kit_manifest.get("presetId"),
        "params": asdict(params),
        "drumEngine": engine,
        "drumEngineLabel": PROFILES.get(engine, PROFILES["mr_tape"]).label,
        "device": {"model": "LIVEN Lofi-12", "targetBank": "A", "sampleRateHz": dst_rate},
        "slots": entries,
    }
    (out_dir / "load_manifest.json").write_text(json.dumps(manifest, indent=2))
    return manifest


def main() -> None:
    p = argparse.ArgumentParser(description="Lofi-12 phonk drum bank factory")
    p.add_argument("--prompt", default=DEFAULT_BANK_PROMPT)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--variation", type=int, default=0)
    p.add_argument("--rate", type=int, choices=(12000, 24000), default=24000)
    p.add_argument("--batch", type=int, default=0, help="If >0, write bank_a_01 … bank_a_NN subfolders")
    p.add_argument(
        "--from-disklordz",
        type=Path,
        default=None,
        help="Folder with Disklordz manifest.json + WAVs; fills mapped slots, synths the rest",
    )
    p.add_argument("--engine", default=None, help="Drum machine engine (see phonk_machine_engine.py)")
    preset_help = ", ".join(list_preset_ids())
    p.add_argument(
        "--preset",
        default=None,
        help=f"Memphis style preset for bank tone ({preset_help})",
    )
    args = p.parse_args()

    prompt_was_default = args.prompt == DEFAULT_BANK_PROMPT
    preset = get_preset(args.preset) if args.preset else None
    if preset:
        if prompt_was_default:
            args.prompt = preset.prompt
        if args.engine is None and preset.engine:
            args.engine = preset.engine

    if args.batch > 0:
        for i in range(args.batch):
            sub = args.out / f"bank_a_{i + 1:02d}"
            generate_bank(
                prompt=args.prompt,
                out_dir=sub,
                variation=i,
                dst_rate=args.rate,
                engine_id=args.engine,
            )
            print(f"Wrote {sub}")
        return

    if args.from_disklordz:
        manifest = import_disklordz_kit(
            args.from_disklordz,
            args.out,
            prompt=args.prompt,
            dst_rate=args.rate,
            engine_id=args.engine,
        )
    else:
        manifest = generate_bank(
            prompt=args.prompt,
            out_dir=args.out,
            variation=args.variation,
            dst_rate=args.rate,
            engine_id=args.engine,
        )
    print(json.dumps({"out": str(args.out), "slots": len(manifest["slots"])}, indent=2))


if __name__ == "__main__":
    main()
