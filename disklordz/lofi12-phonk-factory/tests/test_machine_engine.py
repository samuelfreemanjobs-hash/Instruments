"""Drum machine engine tests."""

from __future__ import annotations

import hashlib
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from phonk_machine_engine import ENGINE_IDS, render_machine_voice, resolve_engine_id  # noqa: E402
from phonk_synth import resolve_phonk_params  # noqa: E402


class MachineEngineTests(unittest.TestCase):
    def test_resolve_keywords(self) -> None:
        self.assertEqual(resolve_engine_id("juicy j memphis dirty"), "juicy_j")
        self.assertEqual(resolve_engine_id("dj toomp atlanta trap"), "dj_toomp")
        self.assertEqual(resolve_engine_id("pure tr909 beat"), "tr909")
        self.assertEqual(resolve_engine_id("boss dr660"), "boss_dr660")
        self.assertEqual(resolve_engine_id("splice tape pack"), "mr_tape")

    def test_engines_render_kick(self) -> None:
        params = resolve_phonk_params("memphis phonk", 0)
        digests: set[str] = set()
        for eid in ENGINE_IDS:
            if eid == "classic":
                continue
            pcm = render_machine_voice(eid, "kick_808", params)
            self.assertGreater(len(pcm), 500)
            digests.add(hashlib.sha256(bytes(bytearray(int(x * 127) & 0xFF for x in pcm[:800]))).hexdigest()[:12])
        self.assertGreater(len(digests), 4, "engines should differ audibly")


if __name__ == "__main__":
    unittest.main()
