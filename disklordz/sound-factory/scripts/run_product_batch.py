#!/usr/bin/env python3
"""Batch-render catalog slots with QA + SFZ (+ optional project bundle)."""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from emit_sfz import emit_sfz
from factory_qa import analyze_wav
from generate_multisample_instrument import build_zones
from rev2trap_render import build_zones_rev2trap
from trap_synth import ProducerLane

try:
    from dawdreamer_render import build_zones_dawdreamer
except ImportError:
    build_zones_dawdreamer = None  # type: ignore[misc, assignment]

ROOT = Path(__file__).resolve().parents[1]


def export_project_bundle(inst_dir: Path, product_id: str, slot: dict) -> None:
    bundle = {
        "format": "DISKLORDZ_FACTORY_PROJECT",
        "version": 1,
        "productId": product_id,
        "slot": slot,
        "exports": {
            "instrumentMap": str(inst_dir / "instrument-map.json"),
            "sfz": str(inst_dir / f"{slot['instrumentId']}.sfz"),
            "mpcHandoff": "Run cursor-mpc-agent on this folder (see HANDOFF_MPC.md)",
        },
    }
    (inst_dir / "project.json").write_text(json.dumps(bundle, indent=2), encoding="utf-8")
    handoff = inst_dir / "HANDOFF_MPC.md"
    handoff.write_text(
        f"# MPC handoff — {slot['instrumentId']}\n\n"
        f"- instrument-map.json + samples/\n"
        f"- SFZ: `{slot['instrumentId']}.sfz`\n"
        f"- Delegate to **Cursor MPC-agent** + MPCTK for Keygroup `.xpj`\n",
        encoding="utf-8",
    )


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--catalog", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--offset", type=int, default=0)
    p.add_argument("--limit", type=int, default=10)
    p.add_argument("--fail-fast", action="store_true")
    p.add_argument(
        "--engine",
        choices=("python", "rev2trap", "dawdreamer", "auto"),
        default="auto",
        help="auto: rev2trap for REV2/JZ400 when binary exists, else python; "
        "dawdreamer needs --vst-path or FACTORY_VST_PATH",
    )
    p.add_argument("--rev2-binary", type=Path, default=None)
    p.add_argument("--vst-path", type=Path, default=None, help="VST/VST3/CLAP for dawdreamer engine")
    p.add_argument("--best-of", type=int, default=3)
    p.add_argument("--skip-sfz-verify", action="store_true")
    p.add_argument("--spectral-gate", action="store_true")
    args = p.parse_args()

    catalog = json.loads(args.catalog.read_text(encoding="utf-8"))
    product_id = catalog["productId"]
    slots = catalog["slots"][args.offset : args.offset + args.limit]

    def pick_engine() -> str:
        if args.engine != "auto":
            return args.engine
        if product_id in ("REV2-TRAP-128", "JZ400"):
            try:
                from rev2trap_render import find_binary

                find_binary(args.rev2_binary)
                return "rev2trap"
            except FileNotFoundError:
                pass
        if args.vst_path or os.environ.get("FACTORY_VST_PATH"):
            return "dawdreamer"
        return "python"

    engine = pick_engine()
    if engine == "dawdreamer" and build_zones_dawdreamer is None:
        raise RuntimeError("dawdreamer engine unavailable (import failed)")
    summary = {"productId": product_id, "engine": engine, "rendered": [], "failed": []}

    for slot in slots:
        inst_id = slot["instrumentId"]
        lane: ProducerLane = slot.get("lane", "jeezy")
        inst_dir = args.out / product_id / inst_id
        try:
            if engine == "rev2trap":
                imap = build_zones_rev2trap(
                    inst_dir, inst_id, slot, args.rev2_binary, best_of=max(1, args.best_of)
                )
            elif engine == "dawdreamer":
                imap = build_zones_dawdreamer(
                    inst_dir, inst_id, slot, vst_path=args.vst_path
                )
            else:
                seed = int(slot["slotId"].split("_")[-1]) if "_" in slot["slotId"] else 0
                imap = build_zones(inst_dir, inst_id, lane, seed)
            (inst_dir / "instrument-map.json").write_text(json.dumps(imap, indent=2), encoding="utf-8")
            emit_sfz(imap, inst_dir)
            for z in imap["zones"]:
                wav = inst_dir / z["samplePath"]
                qa = analyze_wav(wav)
                if not qa.pass_qa:
                    raise RuntimeError(qa.reasons)
            if not args.skip_sfz_verify:
                from verify_sfz_roundtrip import verify

                sfz_err = verify(inst_dir)
                if sfz_err:
                    raise RuntimeError(sfz_err)
            if args.spectral_gate:
                from factory_spectral_gate import gate

                cat = slot.get("category") or slot.get("archetype") or "trap_lead"
                probe = inst_dir / imap["zones"][len(imap["zones"]) // 2]["samplePath"]
                if not gate(probe, str(cat)):
                    raise RuntimeError("spectral_gate_failed")
            export_project_bundle(inst_dir, product_id, slot)
            summary["rendered"].append(inst_id)
        except Exception as e:
            summary["failed"].append({"instrumentId": inst_id, "error": str(e)})
            if args.fail_fast:
                break

    report_path = args.out / product_id / f"batch_report_{args.offset}_{args.limit}.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(json.dumps(summary, indent=2), encoding="utf-8")
    print(json.dumps(summary, indent=2))
    if summary["failed"] and args.fail_fast:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
