#!/usr/bin/env python3
"""Contract tests for the design-system checker and theme scaffolder.

Each test breaks one rule and expects a failure (or proves a guarantee).
Run: python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"
"""
from __future__ import annotations

import copy
import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import check_tokens  # noqa: E402
import new_theme  # noqa: E402
from check_tokens import check, contrast, list_themes, load_contract, load_theme  # noqa: E402


class ThemeContracts(unittest.TestCase):
    def setUp(self) -> None:
        self.contract = load_contract()
        self.tokens = load_theme("disklordz")

    def errors_after(self, mutate, tokens: dict | None = None) -> list[str]:
        t = copy.deepcopy(tokens or self.tokens)
        mutate(t)
        return check(t, self.contract)

    def test_every_shipped_theme_passes(self):
        """Contract: every theme in themes/ satisfies the role contract."""
        themes = list_themes()
        self.assertIn("disklordz", themes)
        self.assertIn("_template", themes)
        self.assertGreaterEqual(len(themes), 3, "need at least one non-brand theme to prove swapability")
        for theme_id in themes:
            with self.subTest(theme=theme_id):
                self.assertEqual(check(load_theme(theme_id), self.contract), [])

    def test_themes_cover_identical_roles(self):
        """Contract: components can switch theme without code changes (same semantic role set)."""
        def roles(t):
            return sorted(p for p, _ in check_tokens.leaves(t["semantic"], "semantic"))
        reference = roles(self.tokens)
        for theme_id in list_themes():
            with self.subTest(theme=theme_id):
                self.assertEqual(roles(load_theme(theme_id)), reference)

    def test_missing_role_fails(self):
        """Contract: a theme must define every required role."""
        errs = self.errors_after(lambda t: t["semantic"]["meter"].pop("peak"))
        self.assertIn("missing required role semantic.meter.peak", errs)

    def test_theme_cannot_loosen_contract(self):
        """Contract: theme contrast pairs may tighten, never loosen, the baseline."""
        def mutate(t):
            t["contrast"].append({"fg": "semantic.text.primary", "bg": "semantic.bg.panel", "min": 3.0, "use": "x"})
        errs = self.errors_after(mutate)
        self.assertTrue(any("loosens contrast" in e for e in errs), errs)

    def test_contract_major_mismatch_fails(self):
        """Contract: themes declare which contract major version they implement."""
        errs = self.errors_after(lambda t: t.__setitem__("contract", "2.0.0"))
        self.assertTrue(any("targets contract" in e for e in errs), errs)

    def test_header_fields_required(self):
        """Contract: id, name, brandName, version, contract are required."""
        errs = self.errors_after(lambda t: t.pop("brandName"))
        self.assertIn("theme header missing 'brandName'", errs)

    def test_reserved_colour_outside_role_fails(self):
        """Contract: a theme's usageRestrictions are enforced (DiskLordz cyan = meters only)."""
        errs = self.errors_after(lambda t: t["semantic"]["brand"].__setitem__("stroke", "{color.cyan.400}"))
        self.assertTrue(any("uses color.cyan" in e for e in errs), errs)

    def test_low_contrast_fails(self):
        """Contract: baseline contrast minimums are enforced."""
        errs = self.errors_after(lambda t: t["color"]["neutral"].__setitem__("100", "#2A2A2A"))
        self.assertTrue(any(e.startswith("contrast semantic.text.primary") for e in errs), errs)

    def test_unresolved_reference_fails(self):
        """Contract: every reference resolves."""
        errs = self.errors_after(lambda t: t["semantic"]["bg"].__setitem__("panel", "{color.neutral.123}"))
        self.assertTrue(any("unresolved reference" in e for e in errs), errs)

    def test_reference_cycle_fails(self):
        """Contract: reference cycles are rejected, not looped forever."""
        def mutate(t):
            t["semantic"]["bg"]["panel"] = "{semantic.bg.section}"
            t["semantic"]["bg"]["section"] = "{semantic.bg.panel}"
        errs = self.errors_after(mutate)
        self.assertTrue(any("cycle" in e for e in errs), errs)

    def test_bad_hex_fails(self):
        """Contract: colours are #RRGGBB."""
        errs = self.errors_after(lambda t: t["color"]["lime"].__setitem__("500", "lime"))
        self.assertTrue(any("not a #RRGGBB" in e for e in errs), errs)

    def test_off_grid_spacing_fails(self):
        """Contract: spacing is a multiple of the contract grid."""
        errs = self.errors_after(lambda t: t["space"].__setitem__("2", 6))
        self.assertTrue(any("space.2" in e for e in errs), errs)

    def test_text_floor_is_contract_level(self):
        """Contract: a theme cannot lower minSize below the contract floor."""
        def mutate(t):
            t["typography"]["minSize"] = 8
            t["typography"]["style"]["scale"]["size"] = 8
        errs = self.errors_after(mutate)
        self.assertTrue(any("contract floor" in e for e in errs), errs)

    def test_small_hit_target_fails(self):
        """Contract: interactive controls meet hitTargetMin."""
        errs = self.errors_after(lambda t: t["size"]["button"].__setitem__("sm", 20))
        self.assertTrue(any("size.button.sm" in e for e in errs), errs)

    def test_contrast_formula_reference_values(self):
        """Contract: WCAG 2.x formula (black/white = 21, identical = 1)."""
        self.assertAlmostEqual(contrast("#000000", "#FFFFFF"), 21.0, places=2)
        self.assertAlmostEqual(contrast("#7B2CBF", "#7B2CBF"), 1.0, places=6)


class NewThemeScaffold(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp())
        shutil.copytree(check_tokens.THEMES / "_template", self.tmp / "_template")
        self._orig = new_theme.THEMES
        new_theme.THEMES = self.tmp

    def tearDown(self) -> None:
        new_theme.THEMES = self._orig
        shutil.rmtree(self.tmp)

    def test_scaffolded_theme_passes_immediately(self):
        """Contract: new_theme.py output is a valid theme with the requested identity."""
        dst = new_theme.create("acme", "Acme Audio", "Acme")
        tokens = json.loads((dst / "tokens.json").read_text(encoding="utf-8"))
        self.assertEqual((tokens["id"], tokens["name"], tokens["brandName"]), ("acme", "Acme Audio", "Acme"))
        self.assertEqual(check(tokens, load_contract()), [])

    def test_bad_ids_rejected(self):
        """Contract: theme ids are lowercase-kebab and cannot shadow the starter."""
        for bad in ("Acme", "_private", "acme audio"):
            with self.subTest(theme_id=bad), self.assertRaises(ValueError):
                new_theme.create(bad, "x", "x")

    def test_existing_theme_not_overwritten(self):
        """Contract: scaffolding never overwrites an existing theme."""
        new_theme.create("acme", "Acme", "Acme")
        with self.assertRaises(FileExistsError):
            new_theme.create("acme", "Acme", "Acme")


if __name__ == "__main__":
    unittest.main()
