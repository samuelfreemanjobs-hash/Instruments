#!/usr/bin/env python3
"""Package DiskLordz bass oscillator bundle + 14 single-SKU products."""
from __future__ import annotations

import json
import shutil
import sys
import zipfile
from datetime import datetime, timezone
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
ROOT = Path(__file__).resolve().parents[2]
CATALOG_PATH = ROOT / "disklordz/products/custom-bass-oscillators/catalog.json"
LEGAL = ROOT / "disklordz/products/custom-bass-oscillators/legal/EULA_PLACEHOLDER.md"
BUNDLE_PRODUCT_SRC = ROOT / "disklordz/products/custom-bass-oscillators/PRODUCT.md"

# Reuse doc helpers from generate script
from generate_bass_osc_product_docs import (  # noqa: E402
    compatibility_md,
    format_param_table,
    format_presets,
    internal_notes,
    load_catalog,
    manifest_path,
    oscillators_md,
    quick_start_md,
)


def find_unit(src_dir: Path, name: str) -> Path | None:
    for p in (src_dir / name, src_dir / "build" / name):
        if p.is_file():
            return p
    return None


def copy_unit_four_platforms(root: Path, unit: dict, units_root: Path) -> list[str]:
    """Copy 4 platform files into units_root/{nts-1,...}/. Returns relative paths."""
    copied: list[str] = []
    v1 = root / "src" / unit["v1_dir"]
    proj = unit["project"]
    for ext, sub in (
        ("ntkdigunit", "nts-1"),
        ("mnlgxdunit", "minilogue-xd"),
        ("prlgunit", "prologue"),
    ):
        src = find_unit(v1, f"{proj}.{ext}")
        if src is None:
            raise FileNotFoundError(f"{proj}.{ext}")
        dest = units_root / sub / src.name
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dest)
        copied.append(str(dest.relative_to(units_root.parent)))
    mkii = root / "src/mkii/oscillators" / unit["mkii_slug"]
    mkii_name = f"{unit['mkii_slug']}.nts1mkiiunit"
    src = find_unit(mkii, mkii_name)
    if src is None:
        raise FileNotFoundError(mkii_name)
    dest = units_root / "nts-1-mkii" / src.name
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dest)
    copied.append(str(dest.relative_to(units_root.parent)))
    return copied


def load_manifest(unit: dict) -> dict:
    return json.loads(manifest_path(unit["v1_dir"]).read_text(encoding="utf-8"))


def dsp_bullets(unit: dict, manifest: dict) -> list[str]:
    header = manifest["header"]
    agent = manifest.get("_agent", {})
    desc = agent.get("description", unit["store_blurb"])
    bullets = [desc]
    param_names = ", ".join(p[0] for p in header["params"])
    bullets.append(
        f"{header['num_param']} panel controls ({param_names}) — ranges in OSCILLATOR.md."
    )
    presets = agent.get("presets") or []
    if presets:
        bullets.append(f"{len(presets)} documented preset knob recipes in OSCILLATOR.md.")
    targets = agent.get("targets") or ["nts-1", "minilogue-xd", "prologue"]
    bullets.append(
        f"Ships for NTS-1 (mkI), minilogue xd, prologue, and NTS-1 mkII ({len(targets)}+ supported targets in manifest)."
    )
    return bullets


def single_quick_start(catalog: dict, unit: dict) -> str:
    return f"""# Quick start — {unit['store_name']}

**SKU:** {unit['sku']} · **Version:** {catalog['version']}

## Files (one per platform)

| Device | Path |
|--------|------|
| NTS-1 (mkI) | `units/nts-1/{unit['project']}.ntkdigunit` |
| minilogue xd | `units/minilogue-xd/{unit['project']}.mnlgxdunit` |
| prologue | `units/prologue/{unit['project']}.prlgunit` |
| NTS-1 mkII | `units/nts-1-mkii/{unit['mkii_slug']}.nts1mkiiunit` |

## Load

1. Install **KORG KONTROL Editor** (or your synth's librarian).
2. USB-connect the hardware.
3. Open the **custom oscillator** slot → **Load** the file for your device (table above).
4. **Write** to the unit. On NTS-1, only one custom osc is active at a time.

## Sounds

See **[OSCILLATOR.md](OSCILLATOR.md)** for control chart and presets.

## Save money

Own more than one vibe? The full **{catalog['bundle']['store_name']}** ({catalog['bundle']['sku']}) includes all 14 oscillators — see PRODUCT.md.
"""


def single_oscillator_md(unit: dict) -> str:
    data = load_manifest(unit)
    header = data["header"]
    agent = data.get("_agent", {})
    display = header.get("name", unit["store_name"])
    parts = [
        f"# {unit['store_name']}\n",
        f"- **SKU:** {unit['sku']}\n",
        f"- **In-unit display name:** {display}\n",
        f"- **Role:** {agent.get('description', unit['store_blurb'])}\n\n",
        "## Controls\n\n",
        format_param_table(header["params"]),
        "\n\n## Suggested presets (manifest)\n\n",
        format_presets(agent.get("presets", [])),
        "\nKnob values are 0–100 (or 0–2 for discrete types), left to right on the panel.\n",
        "NTS-1 mkII uses 0–1023 internally; use these as starting points.\n",
    ]
    return "".join(parts)


def single_compatibility(catalog: dict, unit: dict) -> str:
    return f"""# Compatibility — {unit['store_name']}

| Platform | Included |
|----------|----------|
| NTS-1 (mkI) | ✓ |
| minilogue xd | ✓ |
| prologue | ✓ |
| NTS-1 mkII | ✓ |

**Pack version:** {catalog['version']}  
Requires Korg logue-compatible firmware and KONTROL Editor (or equivalent).
"""


def single_product_card(catalog: dict, unit: dict, manifest: dict) -> str:
    bundle = catalog["bundle"]
    bullets = dsp_bullets(unit, manifest)
    bullet_md = "\n".join(f"- {b}" for b in bullets)
    short = (
        f"> **{unit['store_name']}** — {unit['store_blurb']} "
        f"Four loadable files: NTS-1, xd, Prologue, mkII."
    )
    long = f"""## {unit['store_name']}

{unit['store_blurb']}

You get **one custom oscillator**, not a sample pack — load it into your Korg's user osc slot and play it like factory firmware. This download includes **all four platform binaries** so the same patch logic follows your board.

### What you get (DSP)

{bullet_md}

### Short store line

{short}

### Long store line

{unit['store_name']} is a single logue SDK user oscillator tuned for **low-end and bass focus**. Macros map to the real panel controls listed in OSCILLATOR.md (sourced from the unit manifest — no invented parameters). Use host cutoff/resonance on NTS-1 where the osc expects it.

### Suggested price (suggestion only)

**${unit['suggested_price_usd']:.2f}** — one specialty osc; compare to the full **{bundle['store_name']}** at **${bundle['suggested_price_usd']:.2f}** ({bundle['sku']}) when you want the whole genre library.

### Bundle upsell

**Upgrade path:** [{bundle['store_name']}]({bundle['sku']}) includes this oscillator plus 13 more bass/kick units, same four-platform layout. Ideal when you swap oscs per track rather than buying à la carte.

### Trademark note

Marketing names are generic; in-device display strings may differ. Third-party brand names are not used in this product card.
"""
    return f"""# PRODUCT — {unit['store_name']}

**SKU:** `{unit['sku']}` · **Version:** {catalog['version']}

{long}
"""


def write_legal(out_dir: Path) -> None:
    legal_out = out_dir / "legal"
    legal_out.mkdir(parents=True, exist_ok=True)
    if LEGAL.is_file():
        (legal_out / "EULA_PLACEHOLDER.md").write_text(
            LEGAL.read_text(encoding="utf-8"), encoding="utf-8"
        )


def write_bundle(root: Path, catalog: dict, products_dir: Path) -> tuple[Path, Path]:
    version = catalog["version"]
    name = f"disklordz-custom-bass-oscillators-{version}"
    out = products_dir / name
    if out.exists():
        shutil.rmtree(out)
    units_root = out / "units"
    for sub in ("nts-1", "minilogue-xd", "prologue", "nts-1-mkii"):
        (units_root / sub).mkdir(parents=True, exist_ok=True)

    all_files: list[str] = []
    for unit in catalog["units"]:
        paths = copy_unit_four_platforms(root, unit, units_root)
        all_files.extend(paths)

    docs = out / "docs"
    docs.mkdir(parents=True, exist_ok=True)
    (docs / "QUICK_START.md").write_text(quick_start_md(catalog), encoding="utf-8")
    (docs / "OSCILLATORS.md").write_text(oscillators_md(catalog), encoding="utf-8")
    (docs / "COMPATIBILITY.md").write_text(compatibility_md(catalog), encoding="utf-8")
    (docs / "INTERNAL_RELEASE_NOTES.md").write_text(internal_notes(), encoding="utf-8")
    write_legal(out)
    if BUNDLE_PRODUCT_SRC.is_file():
        (out / "PRODUCT.md").write_text(
            BUNDLE_PRODUCT_SRC.read_text(encoding="utf-8"), encoding="utf-8"
        )
    (out / "README.md").write_text(
        f"# {catalog['bundle']['store_name']} v{version}\n\n"
        "Start with **[docs/QUICK_START.md](docs/QUICK_START.md)**.\n",
        encoding="utf-8",
    )
    (out / "VERSION").write_text(version + "\n", encoding="utf-8")
    (out / "SKU.txt").write_text(f"{catalog['bundle']['sku']}\n", encoding="utf-8")

    inv = {
        "type": "bundle",
        "sku": catalog["bundle"]["sku"],
        "version": version,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "unit_files": all_files,
    }
    (out / "MANIFEST.json").write_text(json.dumps(inv, indent=2) + "\n", encoding="utf-8")

    zip_path = products_dir / f"{name}.zip"
    _zip_dir(out, zip_path)
    return out, zip_path


def write_single(root: Path, catalog: dict, unit: dict, products_dir: Path) -> tuple[Path, Path]:
    version = catalog["version"]
    folder = f"disklordz-{unit['product_slug']}-{version}"
    out = products_dir / "singles" / folder
    if out.exists():
        shutil.rmtree(out)
    units_root = out / "units"
    copied = copy_unit_four_platforms(root, unit, units_root)

    manifest = load_manifest(unit)
    docs = out / "docs"
    docs.mkdir(parents=True, exist_ok=True)
    (docs / "QUICK_START.md").write_text(single_quick_start(catalog, unit), encoding="utf-8")
    (docs / "OSCILLATOR.md").write_text(single_oscillator_md(unit), encoding="utf-8")
    (docs / "COMPATIBILITY.md").write_text(single_compatibility(catalog, unit), encoding="utf-8")
    write_legal(out)
    (out / "PRODUCT.md").write_text(
        single_product_card(catalog, unit, manifest), encoding="utf-8"
    )
    (out / "README.md").write_text(
        f"# {unit['store_name']}\n\n**SKU:** {unit['sku']}\n\n"
        "→ [docs/QUICK_START.md](docs/QUICK_START.md)\n",
        encoding="utf-8",
    )
    (out / "VERSION").write_text(version + "\n", encoding="utf-8")
    (out / "SKU.txt").write_text(f"{unit['sku']}\n", encoding="utf-8")

    inv = {
        "type": "single",
        "sku": unit["sku"],
        "version": version,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "unit_files": copied,
    }
    (out / "MANIFEST.json").write_text(json.dumps(inv, indent=2) + "\n", encoding="utf-8")

    zip_path = products_dir / "singles" / f"{folder}.zip"
    _zip_dir(out, zip_path)
    return out, zip_path


def _zip_dir(src_dir: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    if zip_path.exists():
        zip_path.unlink()
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for f in sorted(src_dir.rglob("*")):
            if f.is_file():
                zf.write(f, f.relative_to(src_dir.parent))


def write_sku_table(products_dir: Path, catalog: dict, rows: list[dict]) -> Path:
    lines = [
        "# DiskLordz custom bass oscillator — SKU table",
        "",
        f"Generated: {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC')}",
        f"Version: {catalog['version']}",
        "",
        "| SKU | Store name | Suggested price (USD) | Zip |",
        "|-----|------------|----------------------|-----|",
    ]
    b = catalog["bundle"]
    bundle_zip = products_dir / f"disklordz-custom-bass-oscillators-{catalog['version']}.zip"
    lines.append(
        f"| {b['sku']} | {b['store_name']} | {b['suggested_price_usd']:.2f} (suggestion) | `{bundle_zip.name}` |"
    )
    for r in rows:
        lines.append(
            f"| {r['sku']} | {r['store_name']} | {r['price']:.2f} (suggestion) | `{r['zip']}` |"
        )
    lines.extend(
        [
            "",
            "## Output directories",
            "",
            f"- Bundle: `{products_dir / ('disklordz-custom-bass-oscillators-' + catalog['version'])}`",
            f"- Singles: `{products_dir / 'singles'}/`",
            "",
        ]
    )
    path = products_dir / f"SKU_TABLE-{catalog['version']}.md"
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return path


def package_all(root: Path) -> None:
    catalog = load_catalog()
    version = catalog["version"]
    products_dir = root / "release/products"
    products_dir.mkdir(parents=True, exist_ok=True)

    bundle_dir, bundle_zip = write_bundle(root, catalog, products_dir)
    print(f"Bundle: {bundle_dir}")
    print(f"Bundle zip: {bundle_zip}")

    rows: list[dict] = []
    for unit in catalog["units"]:
        out, zpath = write_single(root, catalog, unit, products_dir)
        rows.append(
            {
                "sku": unit["sku"],
                "store_name": unit["store_name"],
                "price": unit["suggested_price_usd"],
                "zip": zpath.relative_to(products_dir).as_posix(),
                "dir": out.relative_to(products_dir).as_posix(),
            }
        )
        print(f"Single {unit['sku']}: {zpath}")

    table = write_sku_table(products_dir, catalog, rows)
    index = {
        "version": version,
        "bundle_zip": bundle_zip.relative_to(root).as_posix(),
        "singles": rows,
        "sku_table": table.relative_to(root).as_posix(),
    }
    (products_dir / f"PRODUCT_INDEX-{version}.json").write_text(
        json.dumps(index, indent=2) + "\n", encoding="utf-8"
    )
    print(f"SKU table: {table}")


if __name__ == "__main__":
    package_all(ROOT)
