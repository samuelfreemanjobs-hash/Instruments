"""Contracts for BLACK BOX 96 generator inventory."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.black_box.generate_era import generate_all, load_profile  # noqa: E402

PROFILE = load_profile("bb96")


class BlackBox96Tests(unittest.TestCase):
    def test_inventory_count_in_band(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            rows = generate_all(Path(td), PROFILE)
            self.assertEqual(len(rows), 480)
            self.assertGreaterEqual(len(rows), 400)
            self.assertLessEqual(len(rows), 500)
            kinds = {r["kind"] for r in rows}
            for need in ("808", "KICK", "SNR", "CLAP", "CH", "COW", "TOM", "LOOP"):
                self.assertIn(need, kinds)

    def test_808_hard_hit_and_long_decay(self) -> None:
        import wave
        import numpy as np

        with tempfile.TemporaryDirectory() as td:
            rows = generate_all(Path(td), PROFILE)
            eights = [r for r in rows if r["kind"] == "808"]
            self.assertEqual(len(eights), 72)
            tags = {r["name"].split("_")[2] for r in eights}
            self.assertGreaterEqual(len(tags), 4)  # Thick/Punch/Crawl/Dirty
            # Spot-check first Thick and a Crawl for duration/tail energy
            by_tag = {}
            for r in eights:
                tag = r["name"].split("_")[2]
                by_tag.setdefault(tag, []).append(r)
            for tag in ("Thick", "Crawl"):
                path = Path(td) / by_tag[tag][0]["path"]
                with wave.open(str(path), "rb") as wf:
                    n = wf.getnframes()
                    sr = wf.getframerate()
                    raw = wf.readframes(n)
                samples = np.frombuffer(raw, dtype=np.int16).astype(np.float32) / 32768.0
                dur = n / float(sr)
                self.assertGreaterEqual(dur, 1.0, f"{tag} too short: {dur:.2f}s")
                tail = samples[-int(0.15 * sr) :]
                # Long decay still has audible energy (not hard-gated)
                self.assertGreater(float(np.sqrt(np.mean(tail**2))), 1e-4, f"{tag} dead tail")
                front = samples[: int(0.008 * sr)]
                self.assertGreater(float(np.max(np.abs(front))), 0.2, f"{tag} soft attack")


if __name__ == "__main__":
    unittest.main()
