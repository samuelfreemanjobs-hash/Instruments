"""DSP unit tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from drum_dsp import sigmoid_saturation  # noqa: E402
from drum_recipe import DrumRecipe, FusionMacros  # noqa: E402


class DrumDspTests(unittest.TestCase):
    def test_saturation_unity_at_origin(self) -> None:
        for k in (0.0, 0.5, 4.0):
            self.assertAlmostEqual(sigmoid_saturation(0.0, k), 0.0, places=9)
        self.assertAlmostEqual(sigmoid_saturation(0.001, 0.0), 0.001, places=6)

    def test_recipe_provenance_deterministic(self) -> None:
        r1 = DrumRecipe(
            "a",
            factory_mode="occult_tape",
            voice_kind="kick",
            random_seed=42,
            fusion=FusionMacros(drive=0.7),
        )
        r2 = DrumRecipe(
            "a",
            factory_mode="occult_tape",
            voice_kind="kick",
            random_seed=42,
            fusion=FusionMacros(drive=0.7),
        )
        self.assertEqual(r1.provenance_hash(), r2.provenance_hash())


if __name__ == "__main__":
    unittest.main()
