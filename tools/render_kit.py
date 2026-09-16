#!/usr/bin/env python3
"""Batch-render a Disklordz kit to WAV + SQLite sample_dna rows."""

from __future__ import annotations

import argparse
import json
import os
import sys
import uuid
from pathlib import Path

import numpy as np
from scipy.io import wavfile

_TOOLS_DIR = Path(__file__).resolve().parent
if str(_TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(_TOOLS_DIR))

from disklordz import audit, synthesis  # noqa: E402
from disklordz.db import connect, init_db, insert_batch, insert_sample, mean_reference_metrics  # noqa: E402


def _notify_activepieces(webhook: str, kit_name: str, files: list[str], batch_id: str) -> None:
    if not webhook:
        return
    try:
        import requests
    except ImportError:
        print("[activepieces] install requests to enable webhooks", file=sys.stderr)
        return
    payload = {
        "kit_name": kit_name,
        "count": len(files),
        "batch_id": batch_id,
        "files": files[:20],
        "source": "disklordz_render_kit",
    }
    try:
        requests.post(webhook, json=payload, timeout=10)
    except OSError as exc:
        print(f"[activepieces] post failed: {exc}", file=sys.stderr)


def _notify_slack(webhook: str, kit_name: str, files: list[str]) -> None:
    if not webhook:
        return
    try:
        import requests
    except ImportError:
        print("[slack] install requests to enable webhooks", file=sys.stderr)
        return
    manifest = "\n".join(f"• `{f}`" for f in files)
    payload = {
        "text": f"Disklordz batch complete: {kit_name}",
        "blocks": [
            {
                "type": "section",
                "text": {"type": "mrkdwn", "text": f"*Kit:* `{kit_name}`\n*Files:*\n{manifest}"},
            }
        ],
    }
    try:
        requests.post(webhook, json=payload, timeout=10)
    except OSError as exc:
        print(f"[slack] post failed: {exc}", file=sys.stderr)


def _render_starter_kit(kit_name: str, out_dir: Path, sr: int, seed: int | None) -> list[dict]:
    rng = np.random.default_rng(seed)
    items: list[dict] = []

    def add(voice: str, audio: np.ndarray, meta: dict) -> None:
        items.append({"voice": voice, "audio": audio, "meta": meta})

    add(
        "808_phonk",
        synthesis.kick_808_phonk(sr=sr, sp1200=True, grit=0.7),
        {"hardware": "SP-1200", "freq_peak": 42.0, "decay_sec": 1.5, "grit_level": 0.7, "bit_depth": 12},
    )
    add(
        "808_trap",
        synthesis.kick_trap(sr=sr, f_base=40.0),
        {"hardware": "DIGITAL_TRAP", "freq_peak": 40.0, "decay_sec": 1.2, "grit_level": 0.9, "bit_depth": 16},
    )
    add(
        "cowbell",
        synthesis.cowbell_memphis(sr=sr, grit=0.7),
        {"hardware": "TR-808", "freq_peak": 587.33, "decay_sec": 0.8, "grit_level": 0.7, "bit_depth": 16},
    )
    add(
        "hat",
        synthesis.hat_crashed(sr=sr, seed=int(rng.integers(0, 2**31))),
        {"hardware": "NOISE", "freq_peak": None, "decay_sec": 0.15, "grit_level": 0.0, "bit_depth": 16},
    )
    add(
        "snare_trap",
        synthesis.snare_trap(sr=sr, seed=int(rng.integers(0, 2**31))),
        {"hardware": "DIGITAL_TRAP", "freq_peak": 180.0, "decay_sec": 0.2, "grit_level": 0.0, "bit_depth": 16},
    )
    add(
        "obsidian_kick",
        synthesis.obsidian_kick(sr=sr),
        {"hardware": "SONIC_FACTORY", "freq_peak": 38.0, "decay_sec": 0.8, "grit_level": 3.2, "bit_depth": 16},
    )
    return items


def _render_808_grid(count: int, sr: int, seed: int | None) -> list[dict]:
    rng = np.random.default_rng(seed)
    items: list[dict] = []
    for _ in range(count):
        f_base = float(rng.uniform(38.0, 52.0))
        decay = float(rng.uniform(1.2, 2.5))
        grit = float(rng.uniform(0.1, 0.6))
        audio = synthesis.kick_808_phonk(sr=sr, f_base=f_base, grit=grit, sp1200=grit > 0.35)
        items.append(
            {
                "voice": "808_phonk",
                "audio": audio,
                "meta": {
                    "hardware": "TR-808" if grit <= 0.35 else "SP-1200",
                    "freq_peak": f_base,
                    "decay_sec": decay,
                    "grit_level": grit,
                    "bit_depth": 12 if grit > 0.35 else 16,
                },
            }
        )
    return items


def main() -> int:
    parser = argparse.ArgumentParser(description="Render Disklordz drum kit WAVs and catalog metadata.")
    parser.add_argument("--kit-name", default=f"KIT_{uuid.uuid4().hex[:6].upper()}")
    parser.add_argument("--mode", choices=("starter", "grid808"), default="starter")
    parser.add_argument("--count", type=int, default=5, help="For grid808: number of 808 variants")
    parser.add_argument("--out", type=Path, default=_TOOLS_DIR / "output" / "samples")
    parser.add_argument("--db", type=Path, default=_TOOLS_DIR / "data" / "disklordz.db")
    parser.add_argument("--sr", type=int, default=synthesis.DEFAULT_SR)
    parser.add_argument("--seed", type=int, default=None)
    parser.add_argument("--refine-grit", action="store_true", help="Apply batch-only grit loop if RMS is low")
    parser.add_argument("--slack-webhook", default=os.environ.get("DISKLORDZ_SLACK_WEBHOOK", ""))
    args = parser.parse_args()

    init_db(args.db)
    args.out.mkdir(parents=True, exist_ok=True)

    if args.mode == "starter":
        items = _render_starter_kit(args.kit_name, args.out, args.sr, args.seed)
    else:
        items = _render_808_grid(args.count, args.sr, args.seed)

    ref_metrics = None
    with connect(args.db) as conn:
        ref_metrics = mean_reference_metrics(conn)

    exported: list[str] = []
    with connect(args.db) as conn:
        batch_id = insert_batch(conn, args.kit_name, notes=f"mode={args.mode}")
        for item in items:
            audio = item["audio"]
            if args.refine_grit:
                audio = audit.refine_until_pass(
                    audio,
                    lambda a, _i: synthesis.apply_memphis_crunch(synthesis.apply_tape_saturation(a)),
                )
            metrics = audit.fingerprint(audio, args.sr)
            ok, reason = audit.passes_basic_qa(audio, args.sr, min_peak=0.1)
            if not ok:
                print(f"[qa] warning: {item['voice']}: {reason}", file=sys.stderr)

            match_score = None
            if ref_metrics:
                match_score = audit.match_score_against_reference(metrics, ref_metrics)

            safe_voice = item["voice"].replace("/", "_")
            filename = f"{args.kit_name}_{safe_voice}_{uuid.uuid4().hex[:6]}.wav"
            path = args.out / filename
            wavfile.write(path, args.sr, synthesis.to_int16(audio))

            meta = item["meta"]
            insert_sample(
                conn,
                batch_id=batch_id,
                voice=item["voice"],
                wave_path=str(path.resolve()),
                hardware_source=meta.get("hardware"),
                freq_peak=meta.get("freq_peak"),
                decay_sec=meta.get("decay_sec"),
                grit_level=meta.get("grit_level"),
                bit_depth=meta.get("bit_depth"),
                sample_rate=args.sr,
                metrics=metrics,
                match_score=match_score,
            )
            exported.append(filename)
            print(
                json.dumps(
                    {
                        "file": filename,
                        "voice": item["voice"],
                        "rms": round(metrics["rms"], 4),
                        "crest": round(metrics["crest_factor"], 2),
                        "match": round(match_score, 1) if match_score is not None else None,
                    }
                )
            )
        conn.commit()

    _notify_slack(args.slack_webhook, args.kit_name, exported)
    print(f"Wrote {len(exported)} samples → {args.out}")
    print(f"Catalog → {args.db} (batch {batch_id})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
