"""Loop FX tests."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_loop_fx import LoopFxParams, apply_loop_fx, fx_to_lofi12_cc  # noqa: E402


class LoopFxTests(unittest.TestCase):
    def test_apply_changes_signal(self) -> None:
        dry = [0.5 if i % 100 == 0 else 0.0 for i in range(8000)]
        wet = apply_loop_fx(dry, LoopFxParams(filter_cutoff=0.3, reverb_send=0.4, drive=0.3), seed=1)
        self.assertNotEqual(dry[:500], wet[:500])

    def test_cc_mapping(self) -> None:
        cc = fx_to_lofi12_cc(LoopFxParams(filter_cutoff=0.5, reverb_send=0.5))
        self.assertEqual(cc["filterCutoff"], 63)

    def test_bitcrush_and_cassette(self) -> None:
        dry = [0.4 * (1 if i % 50 == 0 else 0) for i in range(4000)]
        wet = apply_loop_fx(
            dry,
            LoopFxParams(bitcrush=0.5, cassette=0.4, filter_cutoff=0.6),
            seed=2,
        )
        self.assertNotEqual(dry, wet)


if __name__ == "__main__":
    unittest.main()
