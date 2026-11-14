"""CI guard — factory tree, catalog, and generated kits stay artist-neutral."""

from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from drum_kit_builder import build_kit  # noqa: E402
from factory_modes import FACTORY_MODE_IDS, MODES  # noqa: E402
from factory_neutral_policy import (  # noqa: E402
    scan_catalog,
    scan_factory_tree,
    scan_kit_paths,
)
from phonk_style_presets import all_presets_api  # noqa: E402


class NeutralNamingPolicyTests(unittest.TestCase):
    def test_factory_source_tree(self) -> None:
        violations = scan_factory_tree(ROOT)
        if violations:
            msg = "\n".join(f"  {ctx}: matched {hit}" for ctx, hit in violations[:40])
            extra = f"\n  ... and {len(violations) - 40} more" if len(violations) > 40 else ""
            self.fail(f"Blocked names in factory sources:\n{msg}{extra}")

    def test_product_catalog_listings(self) -> None:
        catalog = ROOT / "products" / "catalog.json"
        violations = scan_catalog(catalog)
        self.assertEqual(violations, [], msg=str(violations))

    def test_factory_mode_and_preset_api_copy(self) -> None:
        blobs: list[tuple[str, str]] = []
        for mid, mode in MODES.items():
            blobs.append((f"mode:{mid}", f"{mid} {mode.title} {mode.prompt}"))
        for item in all_presets_api():
            blobs.append((f"preset:{item['id']}", f"{item['id']} {item['label']} {item['prompt']}"))
        from factory_neutral_policy import scan_text_blobs

        violations = scan_text_blobs(blobs)
        self.assertEqual(violations, [], msg=str(violations))

    def test_generated_kit_paths_and_manifest(self) -> None:
        sample_modes = (
            "memphis_underground",
            "memphis_raw_southern",
            "phonk_trap",
            "atl_trap_2005",
            "occult_tape",
        )
        with tempfile.TemporaryDirectory() as tmp:
            base = Path(tmp)
            for mode_id in sample_modes:
                if mode_id not in FACTORY_MODE_IDS:
                    continue
                out = base / mode_id
                build_kit(mode_id=mode_id, out_dir=out, kit_size="demo", seed=3)
                violations = scan_kit_paths(out)
                self.assertEqual(violations, [], msg=f"{mode_id}: {violations}")


if __name__ == "__main__":
    unittest.main()
