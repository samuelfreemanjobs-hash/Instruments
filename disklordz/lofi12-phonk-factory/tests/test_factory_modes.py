"""Factory mode naming policy."""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from factory_modes import FACTORY_MODE_IDS, MODES  # noqa: E402
from factory_neutral_policy import find_policy_violations  # noqa: E402


class FactoryModeTests(unittest.TestCase):
    def test_occult_tape_present(self) -> None:
        self.assertIn("occult_tape", FACTORY_MODE_IDS)
        self.assertEqual(MODES["occult_tape"].title, "Occult Tape")

    def test_no_blocked_copy(self) -> None:
        for mid, mode in MODES.items():
            blob = f"{mid} {mode.title} {mode.prompt}"
            hits = find_policy_violations(blob)
            self.assertEqual(hits, [], msg=f"mode {mid}: {hits}")


if __name__ == "__main__":
    unittest.main()
