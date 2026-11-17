"""Contracts for BLACK BOX 96 generator inventory."""

from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.black_box.generate_bb96 import generate_all  # noqa: E402

PROFILE = json.loads(
    (ROOT / "kits/black-box-collection/profiles/bb96.json").read_text(encoding="utf-8")
)


class BlackBox96Tests(unittest.TestCase):
    def test_inventory_count_in_band(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            rows = generate_all(Path(td), PROFILE)
            self.assertEqual(len(rows), 240)
            self.assertGreaterEqual(len(rows), 200)
            self.assertLessEqual(len(rows), 250)
            kinds = {r["kind"] for r in rows}
            for need in ("808", "KICK", "SNR", "CLAP", "CH", "COW", "TOM", "LOOP"):
                self.assertIn(need, kinds)


if __name__ == "__main__":
    unittest.main()
