"""Atlanta trap factory modes — neutral naming + synthesis smoke."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from atl_trap_voices import crash_cymbal, kick_atl_round  # noqa: E402
from drum_kit_builder import build_kit  # noqa: E402
from factory_modes import FACTORY_MODE_IDS, MODES, get_mode  # noqa: E402
from phonk_synth import PhonkParams, resolve_phonk_params  # noqa: E402

BANNED = ("trap or die", "snow", "t.i.", "ti ", "producer", "juicy", "toomp", "66.6")


class AtlTrapModeTests(unittest.TestCase):
    def test_modes_registered(self) -> None:
        for mid in ("atl_trap_2005", "trap_hard_2006", "atl_modern"):
            self.assertIn(mid, FACTORY_MODE_IDS)
            self.assertIn(mid, MODES)

    def test_neutral_copy(self) -> None:
        for mid in ("atl_trap_2005", "trap_hard_2006", "atl_modern"):
            m = get_mode(mid)
            blob = f"{mid} {m.title} {m.prompt}".lower()
            for bad in BANNED:
                self.assertNotIn(bad, blob)

    def test_atl_voices_render(self) -> None:
        p = resolve_phonk_params("atlanta trap sparse 72", 0)
        self.assertGreater(len(kick_atl_round(p)), 1000)
        self.assertGreater(len(crash_cymbal(PhonkParams(seed=3))), 5000)

    def test_demo_kit_build(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            manifest = build_kit(mode_id="atl_trap_2005", out_dir=Path(tmp), kit_size="demo", seed=1)
            self.assertGreaterEqual(manifest["oneShotCount"], 20)
            self.assertTrue((Path(tmp) / "MPC_Export").is_dir())
            self.assertTrue(any(Path(tmp).rglob("One-Shots/toms/*.wav")))
            self.assertTrue(any(Path(tmp).rglob("One-Shots/crashes/*.wav")))


if __name__ == "__main__":
    unittest.main()
