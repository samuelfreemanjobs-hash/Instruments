#!/usr/bin/env python3
"""Batch-render catalog slots with QA + SFZ (+ optional project bundle)."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from emit_sfz import emit_sfz
from factory_qa import analyze_wav
from generate_multisample_instrument import build_zones
from trap_synth import ProducerLane

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
    args = p.parse_args()

    catalog = json.loads(args.catalog.read_text(encoding="utf-8"))
    product_id = catalog["productId"]
    slots = catalog["slots"][args.offset : args.offset + args.limit]
    summary = {"productId": product_id, "rendered": [], "failed": []}

    for slot in slots:
        inst_id = slot["instrumentId"]
        lane: ProducerLane = slot.get("lane", "jeezy")
        inst_dir = args.out / product_id / inst_id
        try:
            seed = int(slot["slotId"].split("_")[-1]) if "_" in slot["slotId"] else 0
            imap = build_zones(inst_dir, inst_id, lane, seed)
            (inst_dir / "instrument-map.json").write_text(json.dumps(imap, indent=2), encoding="utf-8")
            emit_sfz(imap, inst_dir)
            for z in imap["zones"]:
                wav = inst_dir / z["samplePath"]
                qa = analyze_wav(wav)
                if not qa.pass_qa:
                    raise RuntimeError(qa.reasons)
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
