#!/usr/bin/env python3
"""Hardware Preset Designer agent — CLI entry for SynthForge batch + prompt flows."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
SYNTH_FORGE = REPO / "tools" / "synth-forge"
if str(SYNTH_FORGE) not in sys.path:
    sys.path.insert(0, str(SYNTH_FORGE))

from synth_forge.generator import generate_batch  # noqa: E402
from synth_forge.models import PromptGenerateRequest  # noqa: E402
from synth_forge.semantic_engine import generate_from_prompt  # noqa: E402
from synth_forge.synths import get_adapter, list_synths  # noqa: E402


def self_test() -> dict[str, object]:
    adapter = get_adapter("minilogue_xd")
    sample = generate_batch("minilogue_xd", 1, category="luxury_west_coast", seed=0)[0]
    blob = adapter.pack(sample.parameters)
    roundtrip = adapter.unpack(blob)
    return {
        "ok": True,
        "synths": len(list_synths()),
        "pack_bytes": len(blob),
        "filter_resonance": roundtrip.filter_resonance,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Hardware Preset Designer (SynthForge)")
    parser.add_argument("--self-test", action="store_true", help="Fast sanity check for CI")
    parser.add_argument("--list-synths", action="store_true")
    parser.add_argument("--synth-id", default="minilogue_xd")
    parser.add_argument("--category", default="generic")
    parser.add_argument("--count", type=int, default=4)
    parser.add_argument("--prompt", default="", help="Natural language patch brief")
    parser.add_argument("--seed", type=int, default=None)
    args = parser.parse_args()

    if args.self_test:
        print(json.dumps(self_test(), indent=2))
        return 0

    if args.list_synths:
        print(json.dumps(list_synths(), indent=2))
        return 0

    get_adapter(args.synth_id)

    if args.prompt.strip():
        presets = generate_from_prompt(
            PromptGenerateRequest(
                prompt=args.prompt.strip(),
                synth_id=args.synth_id,
                count=min(args.count, 32),
            )
        )
        mode = "prompt"
    else:
        presets = generate_batch(
            args.synth_id,
            min(args.count, 64),
            category=args.category,
            seed=args.seed,
        )
        mode = "batch"

    adapter = get_adapter(args.synth_id)
    exports = [
        {
            "id": p.id,
            "name": p.name,
            "bytes": len(adapter.pack(p.parameters)),
            "category": p.parameters.category,
        }
        for p in presets
    ]
    print(
        json.dumps(
            {"mode": mode, "synth_id": args.synth_id, "count": len(presets), "presets": exports},
            indent=2,
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
