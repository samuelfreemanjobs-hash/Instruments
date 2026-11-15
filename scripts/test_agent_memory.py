#!/usr/bin/env python3
"""Contract tests for scripts/agent_memory.py. Run: python3 -m unittest scripts/test_agent_memory.py"""
from __future__ import annotations

import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import agent_memory as am  # noqa: E402

GOOD = dict(area="ci, juce", when="building JUCE on Linux", lesson="needs X11 headers",
            do="install libxrandr-dev", evidence="run 123", confidence="executed")


class MemoryContracts(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp())
        shutil.copy(am.DEFAULT_ROOT / "MEMORY.md", self.tmp / "MEMORY.md")
        text = (self.tmp / "MEMORY.md").read_text(encoding="utf-8")
        # keep the index self-consistent for an empty store: drop hot-list references
        (self.tmp / "MEMORY.md").write_text(
            "\n".join(l for l in text.splitlines() if "_learnings:" not in l and "_incidents:" not in l) + "\n",
            encoding="utf-8")
        os.environ["AGENT_MEMORY_ROOT"] = str(self.tmp)
        am.write_index()

    def tearDown(self) -> None:
        os.environ.pop("AGENT_MEMORY_ROOT", None)
        shutil.rmtree(self.tmp)

    def test_repo_memory_is_valid(self):
        """Contract: the committed memory passes check (format, ids, manifest, no secrets)."""
        os.environ.pop("AGENT_MEMORY_ROOT", None)
        self.assertEqual(am.check(), [])

    def test_add_then_recall_and_check(self):
        """Contract: an added lesson is findable by keyword and keeps memory valid."""
        e = am.add("demo-agent", "learning", "JUCE needs X11 headers on Linux CI", GOOD)
        self.assertEqual(e.ident, "L-0001")
        self.assertIn("L-0001", am.recall(["x11", "linux"], "demo-agent", 5))
        self.assertEqual(am.check(), [])

    def test_ids_increment_per_file(self):
        """Contract: ids run 1..n within each file."""
        am.add("demo-agent", "learning", "first lesson about pluginval", GOOD)
        e2 = am.add("demo-agent", "learning", "second lesson about fonts", GOOD)
        e3 = am.add("demo-agent", "incident", "an incident with ci", GOOD)
        self.assertEqual((e2.ident, e3.ident), ("L-0002", "I-0001"))

    def test_duplicate_title_rejected(self):
        """Contract: near-duplicate lessons are refused (update the existing one instead)."""
        am.add("demo-agent", "learning", "JUCE needs X11 headers on Linux CI", GOOD)
        with self.assertRaises(ValueError):
            am.add("shared", "learning", "JUCE needs X11 headers on Linux CI!", GOOD)

    def test_secret_rejected_on_add(self):
        """Contract: entries that look like secrets are never written."""
        bad = dict(GOOD, evidence="token ghp_" + "a" * 36)
        with self.assertRaises(ValueError):
            am.add("demo-agent", "learning", "a lesson with a token", bad)
        self.assertFalse((self.tmp / "demo-agent_learnings.md").exists())

    def test_secret_in_file_fails_check(self):
        """Contract: check catches secrets pasted into memory files by hand."""
        am.add("demo-agent", "learning", "webhook setup", GOOD)
        p = self.tmp / "demo-agent_learnings.md"
        p.write_text(p.read_text(encoding="utf-8") + "\nhooks.slack.com/services/T0ABC/B0DEF/abcdef123\n", encoding="utf-8")
        self.assertTrue(any("possible secret" in e for e in am.check()))

    def test_missing_fields_and_bad_confidence_rejected(self):
        """Contract: every entry carries all fields and an evidence tag."""
        with self.assertRaises(ValueError):
            am.add("demo-agent", "learning", "no evidence", dict(GOOD, evidence=""))
        with self.assertRaises(ValueError):
            am.add("demo-agent", "learning", "bad tag", dict(GOOD, confidence="verified"))

    def test_stale_manifest_fails_check(self):
        """Contract: the index manifest must match the files."""
        am.add("demo-agent", "learning", "manifest lesson", GOOD)
        idx = self.tmp / "MEMORY.md"
        idx.write_text(idx.read_text(encoding="utf-8").replace("| 1 |", "| 9 |", 1), encoding="utf-8")
        self.assertTrue(any("manifest is stale" in e for e in am.check()))

    def test_promote_requires_existing_target(self):
        """Contract: promotion records where the lesson now lives, and that place must exist."""
        am.add("demo-agent", "learning", "promote me", GOOD)
        e = am.promote("L-0001", "demo-agent", "scripts/agent_memory.py")
        self.assertEqual(e.fields["status"], "promoted:scripts/agent_memory.py")
        self.assertEqual(am.check(), [])
        am.promote("L-0001", "demo-agent", "docs/does-not-exist.md")
        self.assertTrue(any("does not exist" in x for x in am.check()))

    def test_index_line_limit(self):
        """Contract: MEMORY.md stays a short index (<= 200 lines)."""
        idx = self.tmp / "MEMORY.md"
        idx.write_text(idx.read_text(encoding="utf-8") + "\n" * 250, encoding="utf-8")
        self.assertTrue(any("limit 200" in e for e in am.check()))

    def test_bad_agent_id_rejected(self):
        """Contract: agent ids are lowercase-kebab (or 'shared')."""
        with self.assertRaises(ValueError):
            am.add("Demo Agent", "learning", "x lesson", GOOD)


if __name__ == "__main__":
    unittest.main()
