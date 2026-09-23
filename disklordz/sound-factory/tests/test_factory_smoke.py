#!/usr/bin/env python3
"""Smoke tests for sound-factory (stdlib runner if pytest missing)."""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT / "scripts"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, check=True, cwd=SCRIPTS)


def test_seed_catalogs() -> None:
    run([sys.executable, "seed_jz400_catalog.py", "--target", "400"])
    run([sys.executable, "seed_rev2_trap128_catalog.py"])
    jz = json.loads((ROOT / "catalog" / "jz400_catalog.json").read_text())
    rev = json.loads((ROOT / "catalog" / "rev2_trap128_catalog.json").read_text())
    assert len(jz["slots"]) == 400
    assert len(rev["slots"]) == 128


def test_dawdreamer_module_present() -> None:
    sys.path.insert(0, str(SCRIPTS))
    import DAWDreamer_renderer  # noqa: F401

    assert (SCRIPTS / "renderers" / "dawdreamer_renderer.py").is_file()


def test_render_and_sfz() -> None:
    with tempfile.TemporaryDirectory() as tmp:
        out = Path(tmp)
        run(
            [
                sys.executable,
                "generate_multisample_instrument.py",
                "--instrument-id",
                "SMOKE_001",
                "--lane",
                "shawty_redd",
                "--out",
                str(out),
                "--emit-sfz",
            ]
        )
        assert (out / "instrument-map.json").is_file()
        sfz = list(out.glob("*.sfz"))
        assert sfz
        run([sys.executable, "factory_qa.py", str(out / "samples" / "SMOKE_001_root36.wav")])


def main() -> None:
    test_seed_catalogs()
    test_dawdreamer_module_present()
    test_render_and_sfz()
    print("OK sound-factory smoke tests")


if __name__ == "__main__":
    main()
