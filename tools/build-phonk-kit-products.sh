#!/usr/bin/env bash
# Build WooCommerce-ready phonk kit ZIPs from the Lofi-12 factory + SaaS 6-pack CLI.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FACTORY="$ROOT/disklordz/lofi12-phonk-factory"
CATALOG="$FACTORY/products/catalog.json"
OUT_ROOT="$ROOT/release/phonk-kits"
LICENSE="$FACTORY/products/LICENSE.txt"
STAMP="$(date -u +%Y%m%dT%H%M%SZ)"

log() { printf '→ %s\n' "$*"; }

log "unittest (lofi12 phonk factory)…"
python3 -m unittest discover -s "$FACTORY/tests" -q

mkdir -p "$OUT_ROOT"
WORK="$OUT_ROOT/build-$STAMP"
mkdir -p "$WORK"
log "work dir: $WORK"

export ROOT FACTORY CATALOG WORK OUT_ROOT LICENSE STAMP

python3 <<'PY'
import json
import subprocess
import sys
from pathlib import Path

root = Path(__import__("os").environ["ROOT"])
factory = Path(__import__("os").environ["FACTORY"])
catalog_path = Path(__import__("os").environ["CATALOG"])
work = Path(__import__("os").environ["WORK"])
out_root = Path(__import__("os").environ["OUT_ROOT"])
license_src = Path(__import__("os").environ["LICENSE"])
stamp = __import__("os").environ["STAMP"]

catalog = json.loads(catalog_path.read_text())
products = catalog["products"]

for p in products:
    sku = p["sku"]
    dest = work / sku
    dest.mkdir(parents=True, exist_ok=True)

    if p["type"] == "bank":
        cmd = [
            sys.executable,
            str(factory / "scripts/phonk_factory.py"),
            "--out",
            str(dest),
            "--variation",
            str(p.get("variation", 0)),
        ]
        if p.get("preset"):
            cmd.extend(["--preset", p["preset"]])
        if p.get("prompt"):
            cmd.extend(["--prompt", p["prompt"]])
        subprocess.check_call(cmd)

    elif p["type"] == "loops":
        cmd = [
            sys.executable,
            str(factory / "scripts/phonk_loop_factory.py"),
            "--out",
            str(dest),
            "--batch",
            str(p.get("batch", 4)),
            "--variation",
            str(p.get("variation", 0)),
        ]
        if p.get("preset"):
            cmd.extend(["--preset", p["preset"]])
        if p.get("prompt"):
            cmd.extend(["--prompt", p["prompt"]])
        if p.get("bpm"):
            cmd.extend(["--bpm", str(p["bpm"])])
        subprocess.check_call(cmd)

    elif p["type"] == "full_kit":
        cmd = [
            sys.executable,
            str(factory / "scripts/phonk_kit_factory.py"),
            "--out",
            str(dest),
            "--mode",
            p.get("factoryMode") or p.get("preset") or "memphis_underground",
            "--kit-size",
            p.get("kitSize", "demo"),
            "--seed",
            str(p.get("seed", 0)),
        ]
        subprocess.check_call(cmd)

    elif p["type"] == "disklordz_6":
        subprocess.check_call(
            [
                sys.executable,
                str(root / "disklordz/sound-factory/scripts/generate_kit.py"),
                "--preset",
                p["presetId"],
                "--prompt",
                p["prompt"],
                "--out",
                str(dest),
            ]
        )
    else:
        raise SystemExit(f"unknown product type: {p['type']}")

    (dest / "woo_product.json").write_text(
        json.dumps(
            {
                "sku": sku,
                "title": p["title"],
                "type": p["type"],
                "catalogVersion": catalog.get("version", 1),
            },
            indent=2,
        )
    )

    readme = [
        p["title"],
        f"SKU: {sku}",
        f"Generated: {stamp} (UTC)",
        "",
        f"Type: {p['type']}. Factory: disklordz/lofi12-phonk-factory.",
        "",
        "See LICENSE.txt for usage terms.",
    ]
    (dest / "README.txt").write_text("\n".join(readme))
    (dest / "LICENSE.txt").write_text(license_src.read_text())

    zip_path = out_root / f"{sku}.zip"
    subprocess.check_call(["zip", "-qr", str(zip_path), "."], cwd=dest)
    size_kb = zip_path.stat().st_size // 1024
    print(f"zip {zip_path.name} ({size_kb} KiB)")

(out_root / f"build-manifest-{stamp}.json").write_text(
    json.dumps({"stamp": stamp, "products": products, "workDir": str(work)}, indent=2)
)
print(json.dumps({"built": len(products), "work": str(work), "out": str(out_root)}, indent=2))
PY

ln -sfn "$WORK" "$OUT_ROOT/latest"
log "done — WooCommerce uploads: $OUT_ROOT/*.zip"
