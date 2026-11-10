"""Factory mode naming / Occult Tape policy."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from factory_modes import FACTORY_MODE_IDS, MODES  # noqa: E402


BANNED = ("juicy", "toomp", "66.6", "666", "three 6", "dj paul", "channel", "apocalypse")


class FactoryModeTests(unittest.TestCase):
    def test_occult_tape_present(self) -> None:
        self.assertIn("occult_tape", FACTORY_MODE_IDS)
        self.assertEqual(MODES["occult_tape"].title, "Occult Tape")

    def test_no_banned_copy(self) -> None:
        for mid, mode in MODES.items():
            blob = f"{mid} {mode.title} {mode.prompt}".lower()
            for bad in BANNED:
                self.assertNotIn(bad, blob, msg=f"mode {mid} contains {bad}")


if __name__ == "__main__":
    unittest.main()
