#!/usr/bin/env python3
"""Contract tests for tools/apc_templates.py. Run: python3 -m unittest tools/test_apc_templates.py"""
from __future__ import annotations

import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import apc_templates  # noqa: E402


class ApcTemplateContracts(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp())

    def tearDown(self) -> None:
        shutil.rmtree(self.tmp)

    def make(self, phase: str, **kw):
        return apc_templates.instantiate("DemoFx", phase, plugins_dir=self.tmp, category="fx.demo", **kw)

    def test_all_phases_fill_every_placeholder(self):
        """Contract: generated docs contain no {{PLACEHOLDER}} tokens."""
        written = self.make("all")
        self.assertEqual(len(written), len(apc_templates.PHASES["all"]))
        for p in written:
            with self.subTest(file=p.name):
                self.assertNotIn("{{", p.read_text(encoding="utf-8"))

    def test_files_land_where_apc_gate_expects(self):
        """Contract: paths match tools/apc-gate.sh (.ideas/creative-brief, parameter-spec, architecture, design-spec)."""
        self.make("all")
        root = self.tmp / "DemoFx"
        for rel in (".ideas/creative-brief.md", ".ideas/parameter-spec.md", ".ideas/architecture.md",
                    ".ideas/design-spec.md", "DPCS_PHASE0_CONSTITUTION.md", "scripts/verify-phase1.sh"):
            with self.subTest(rel=rel):
                self.assertTrue((root / rel).is_file())

    def test_never_overwrites_without_force(self):
        """Contract: an edited doc is never clobbered by a re-run."""
        self.make("dream")
        brief = self.tmp / "DemoFx" / ".ideas" / "creative-brief.md"
        brief.write_text("edited", encoding="utf-8")
        self.assertEqual(self.make("dream"), [])
        self.assertEqual(brief.read_text(encoding="utf-8"), "edited")
        self.make("dream", force=True)
        self.assertNotEqual(brief.read_text(encoding="utf-8"), "edited")

    def test_values_substituted(self):
        """Contract: name, class, category and theme reach the documents."""
        self.make("dream", plugin_class="synth", theme="studio-light")
        text = (self.tmp / "DemoFx" / "DPCS_PHASE0_CONSTITUTION.md").read_text(encoding="utf-8")
        for needle in ("DemoFx", "synth", "fx.demo", "studio-light"):
            self.assertIn(needle, text)

    def test_bad_name_and_phase_rejected(self):
        """Contract: invalid plugin names and phases fail loudly."""
        with self.assertRaises(ValueError):
            apc_templates.instantiate("demo fx", "dream", plugins_dir=self.tmp)
        with self.assertRaises(ValueError):
            apc_templates.instantiate("DemoFx", "ship-it", plugins_dir=self.tmp)

    def test_every_template_file_exists(self):
        """Contract: the phase table only references templates that exist."""
        for tpl, _ in apc_templates.PHASES["all"]:
            with self.subTest(tpl=tpl):
                self.assertTrue((apc_templates.TEMPLATES / tpl).is_file())


if __name__ == "__main__":
    unittest.main()
