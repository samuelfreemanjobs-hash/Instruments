#!/usr/bin/env python3
"""Generate customer docs for DiskLordz custom bass oscillator product bundle."""
from __future__ import annotations

import json
from pathlib import Path
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[2]
CATALOG = ROOT / "disklordz/products/custom-bass-oscillators/catalog.json"
LEGAL = ROOT / "disklordz/products/custom-bass-oscillators/legal/EULA_PLACEHOLDER.md"
PRODUCT_SRC = ROOT / "disklordz/products/custom-bass-oscillators/PRODUCT.md"


def load_catalog() -> dict:
    return json.loads(CATALOG.read_text(encoding="utf-8"))


def manifest_path(v1_dir: str) -> Path:
    return ROOT / "src" / v1_dir / "manifest.json"


def format_param_table(params: list) -> str:
    lines = ["| Control | Min | Max | Unit |", "|---------|-----|-----|------|"]
    for p in params:
        name, lo, hi, unit = p[0], p[1], p[2], p[3] if len(p) > 3 else ""
        unit_display = unit if unit else "—"
        lines.append(f"| {name} | {lo} | {hi} | {unit_display} |")
    return "\n".join(lines)


def format_presets(presets: list) -> str:
    if not presets:
        return "_No preset table in manifest._\n"
    out = []
    for pr in presets:
        vals = ", ".join(str(v) for v in pr["values"])
        hint = pr.get("midi_hint", "")
        out.append(f"- **{pr['name']}** — knobs (left→right): `{vals}`")
        if hint:
            out.append(f"  - Playing note: {hint}")
    return "\n".join(out) + "\n"


def oscillators_md(catalog: dict) -> str:
    parts = [
        "# Oscillator reference\n",
        f"Product version **{catalog['version']}**. Control ranges come from each unit's `manifest.json` header.\n",
        "**Knob values** in presets are 0–100 (or 0–2 for discrete types) in manifest order, left to right on the hardware panel.\n",
        "**NTS-1 mkII** uses 10-bit internal values (0–1023); load the preset then nudge from these starting points.\n",
        "---\n",
    ]
    for u in catalog["units"]:
        mpath = manifest_path(u["v1_dir"])
        data = json.loads(mpath.read_text(encoding="utf-8"))
        header = data["header"]
        agent = data.get("_agent", {})
        display = header.get("name", u["store_name"])
        parts.append(f"## {u['store_name']}\n")
        parts.append(f"- **In-unit display name:** {display}\n")
        parts.append(f"- **Store listing name:** {u['store_name']}\n")
        parts.append(f"- **Role:** {agent.get('description', u['store_blurb'])}\n")
        parts.append("### Files\n")
        parts.append(f"- NTS-1 (mkI): `units/nts-1/{u['project']}.ntkdigunit`\n")
        parts.append(f"- minilogue xd: `units/minilogue-xd/{u['project']}.mnlgxdunit`\n")
        parts.append(f"- prologue: `units/prologue/{u['project']}.prlgunit`\n")
        parts.append(f"- NTS-1 mkII: `units/nts-1-mkii/{u['mkii_slug']}.nts1mkiiunit`\n")
        parts.append("### Controls\n\n")
        parts.append(format_param_table(header["params"]))
        parts.append("\n\n### Suggested presets (manifest)\n\n")
        parts.append(format_presets(agent.get("presets", [])))
        parts.append("\n---\n")
    return "".join(parts)


def compatibility_md(catalog: dict) -> str:
    lines = [
        "# Compatibility\n",
        f"**Pack version:** {catalog['version']}\n",
        "| Store name | NTS-1 mkI | minilogue xd | prologue | NTS-1 mkII |",
        "|------------|-----------|--------------|----------|------------|",
    ]
    for u in catalog["units"]:
        lines.append(
            f"| {u['store_name']} | ✓ | ✓ | ✓ | ✓ |"
        )
    lines.append(
        "\n**Requirements**\n"
        "- Korg logue-compatible firmware on each device\n"
        "- KORG **KONTROL Editor** (or current Korg librarian for your product line)\n"
        "- One custom oscillator slot per device — load the file for the track you need\n"
        "- TR-808 Kick unit is a **kick/sub** osc, not a sustained bass voice (play low, short notes)\n"
    )
    return "\n".join(lines) + "\n"


def quick_start_md(catalog: dict) -> str:
    return f"""# Quick start — DiskLordz Custom Bass Oscillators

**Version:** {catalog['version']}  
**Oscillators:** {catalog['oscillator_count']}

## 1. Pick your hardware folder

| Device | Folder | File extension |
|--------|--------|----------------|
| NTS-1 (original) | `units/nts-1/` | `.ntkdigunit` |
| minilogue xd | `units/minilogue-xd/` | `.mnlgxdunit` |
| prologue | `units/prologue/` | `.prlgunit` |
| NTS-1 mkII | `units/nts-1-mkii/` | `.nts1mkiiunit` |

## 2. Install (all platforms)

1. Install **KORG KONTROL Editor** (or the librarian listed in your synth's manual).
2. Connect the synth via USB.
3. Open the **Oscillator** user slot (custom oscillator).
4. **Load** the unit file from the matching folder.
5. **Write** / send to the device. Only **one** custom osc is active at a time on NTS-1 — swap per song if needed.

### minilogue xd / prologue

Use the same flow in KONTROL Editor; select the **Custom Oscillator** slot and load the `.mnlgxdunit` or `.prlgunit` file.

### NTS-1 mkII

Load `.nts1mkiiunit` from `units/nts-1-mkii/` via KONTROL Editor or **logue-cli** (see Korg mkII SDK docs).

## 3. Dial in a sound

Open **[OSCILLATORS.md](OSCILLATORS.md)** for control names, ranges, and preset knob values per osc.

## 4. Panel tips

- Many bass units use host **cutoff** and **resonance** — set those on the NTS-1 before judging the patch.
- Preset numbers in docs follow `manifest.json` knob order (left to right).

## Support

See Disklordz.com product page for updates. This pack does not include factory `dev_id` / `unit_id` registration with Korg — see **INTERNAL_RELEASE_NOTES.md** if you are on the release team (not included in customer ZIP marketing).
"""


def internal_notes() -> str:
    return f"""# Internal release notes (not for store copy)

Generated: {datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}

## QA status

- **Hardware validation:** NOT COMPLETED. Units are built in CI / cross-compile only. Do not ship customer-facing copy claiming hardware-tested until Sam signs the checklist in PRODUCT.md.
- **dev_id / unit_id:** Still 0 in headers — required before wide public distribution per Korg developer policy.
- **Trademark:** Store names avoid third-party marks; in-device `header.name` strings may still reference legacy internal names — review before retail.

## Rebuild

From repo root:

```bash
./tools/package-disklordz-bass-product.sh
```
"""


def write_docs(out_dir: Path) -> None:
    catalog = load_catalog()
    docs = out_dir / "docs"
    docs.mkdir(parents=True, exist_ok=True)
    (docs / "QUICK_START.md").write_text(quick_start_md(catalog), encoding="utf-8")
    (docs / "OSCILLATORS.md").write_text(oscillators_md(catalog), encoding="utf-8")
    (docs / "COMPATIBILITY.md").write_text(compatibility_md(catalog), encoding="utf-8")
    (docs / "INTERNAL_RELEASE_NOTES.md").write_text(internal_notes(), encoding="utf-8")
    legal_out = out_dir / "legal"
    legal_out.mkdir(parents=True, exist_ok=True)
    if LEGAL.is_file():
        (legal_out / "EULA_PLACEHOLDER.md").write_text(LEGAL.read_text(encoding="utf-8"), encoding="utf-8")
    if PRODUCT_SRC.is_file():
        (out_dir / "PRODUCT.md").write_text(PRODUCT_SRC.read_text(encoding="utf-8"), encoding="utf-8")
    readme = (
        f"# DiskLordz Custom Bass Oscillators v{catalog['version']}\n\n"
        "Start with **[docs/QUICK_START.md](docs/QUICK_START.md)**.\n"
    )
    (out_dir / "README.md").write_text(readme, encoding="utf-8")
    (out_dir / "VERSION").write_text(catalog["version"] + "\n", encoding="utf-8")


if __name__ == "__main__":
    import sys

    if len(sys.argv) == 2:
        write_docs(Path(sys.argv[1]))
        print(f"Wrote docs under {sys.argv[1]}")
    elif len(sys.argv) == 1:
        from package_bass_catalog import package_all

        package_all(ROOT)
    else:
        print(
            f"Usage: {sys.argv[0]} [<bundle-output-dir>]\n"
            "  (no args) = full bundle + singles via package_bass_catalog",
            file=sys.stderr,
        )
        raise SystemExit(1)
