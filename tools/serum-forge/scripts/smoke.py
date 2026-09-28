#!/usr/bin/env python3
from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from serum_forge.dx7 import Dx7IncompatibilityError, reject_dx7_sysex  # noqa: E402
from serum_forge.guardrails import clamp_patch, headroom_compensation_db  # noqa: E402
from serum_forge.models import PatchArchetype, SerumSymbolPatch  # noqa: E402
from serum_forge.pipeline import run_pipeline  # noqa: E402


def main() -> int:
    assert headroom_compensation_db(4) < 0

    p = clamp_patch(
        SerumSymbolPatch(
            name="Test Bass",
            archetype=PatchArchetype.bass_sub,
            osc_a_unison_voices=4,
            osc_a_level=0.95,
        )
    )
    assert p.osc_a_level < 0.95
    assert p.fx_reverb_wet <= 0.4

    try:
        reject_dx7_sysex(bytes([0xF0, 0x43]) + bytes(160))
    except Dx7IncompatibilityError:
        pass
    else:
        raise AssertionError("expected DX7 rejection")

    r = run_pipeline("dusty lofi pad", out_dir=Path("/tmp/serum-forge-smoke"))
    assert r.intermediate_json and r.intermediate_json.is_file()

    print("serum-forge smoke: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
