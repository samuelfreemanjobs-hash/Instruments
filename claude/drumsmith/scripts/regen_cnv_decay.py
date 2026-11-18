#!/usr/bin/env python3
"""Re-bake Converter Night masters with current genre recipes (longer decays).

Updates recipe_params in-place, re-renders WAVs via native drumsmith-render,
refreshes listen-web oneshot copies, re-runs technical QA. release_ready stays false.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

from diversity_audit import audit
from genre_profiles import genre_recipe
from recipe_engine import recipe_hash, validate_recipe
from verify_kit import verify

ROOT = Path(__file__).resolve().parents[1]
KIT = ROOT / "kits" / "converter-night"
KINDS = {"808": 0, "KICK": 1, "SNARE": 2, "CLAP": 3, "HAT": 4, "COWBELL": 5, "PERC": 6, "FX": 7}
LISTEN_KIND_DIR = {
    "808": "subs",
    "KICK": "kicks",
    "SNARE": "snares",
    "CLAP": "claps",
    "HAT": "hats",
    "FX": "fx",
}


def _variant_index(samples: list[dict]) -> dict[str, int]:
    """Match build_genre_kit: per-kind sequential variant from scaffold order."""
    counts: dict[str, int] = {}
    out: dict[str, int] = {}
    for e in samples:
        kind = e["kind"]
        n = counts.get(kind, 0)
        counts[kind] = n + 1
        out[e["id"]] = n
    return out


def regen(renderer: Path, listen_only: bool = False) -> dict:
    man_path = KIT / "manifest.json"
    manifest = json.loads(man_path.read_text(encoding="utf8"))
    samples = manifest["samples"]
    variants = _variant_index(samples)
    genre = manifest.get("genre") or "memphis-tape"

    listen_names = set(
        re.findall(r"CNV_[A-Z0-9_]+\.wav", (KIT / "listen-web" / "index.html").read_text(encoding="utf8"))
    )

    rendered = 0
    updated = 0
    for e in samples:
        kind = e["kind"]
        variant = variants[e["id"]]
        # Prefer stored variant if present and consistent with sequential index
        stored = (e.get("recipe_params") or {}).get("variant")
        if isinstance(stored, int) and stored == variant:
            pass
        recipe = genre_recipe(kind, variant, genre)
        validate_recipe(recipe)
        e["recipe_params"] = recipe
        e["recipe_sha256"] = recipe_hash(recipe)
        e["recipe"] = "parametric-v2"
        e["duration_min_sec"] = recipe["duration_sec"] - 0.02
        e["duration_max_sec"] = recipe["duration_sec"] + 0.02
        if kind == "808":
            e["pitch_target_hz"] = recipe["pitch_hz"]
        updated += 1

        name = Path(e["path"]).name
        if listen_only and name not in listen_names:
            continue

        dst = KIT / e["path"]
        dst.parent.mkdir(parents=True, exist_ok=True)
        # drumsmith-render refuses overwrite — stage then replace
        tmp = dst.with_suffix(".wav.tmp")
        if tmp.exists():
            tmp.unlink()
        r = recipe
        cmd = [
            str(renderer),
            str(tmp),
            str(KINDS[kind]),
            str(r["pitch_hz"] or 52),
            str(e["seed"]),
            str(r["decay_sec"]),
            str(r["drive"]),
            str(r["tone"]),
            str(r["noise_color"]),
            str(r["duration_sec"]),
            str(e["sample_rate"]),
        ]
        if kind == "FX":
            cmd.append(str(int(r.get("variant", 0)) % 5))
        subprocess.run(cmd, check=True, capture_output=True, text=True, timeout=35)
        tmp.replace(dst)
        e["sha256"] = hashlib.sha256(dst.read_bytes()).hexdigest()
        rendered += 1

        # Sync listen-web oneshots when this file is in the booth
        if name in listen_names and kind in LISTEN_KIND_DIR:
            dest = KIT / "listen-web" / "audio" / LISTEN_KIND_DIR[kind] / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(dst, dest)

    manifest["status"] = "NATIVE_RENDERED_NOT_RELEASED"
    manifest["generator"] = "drumsmith-cxx-v2-native"
    manifest["human_listening_approved"] = False
    manifest["release_approved"] = False
    man_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf8")

    qa = verify(KIT, man_path)
    diversity = audit(KIT)
    (KIT / "qa-report.json").write_text(json.dumps(qa, indent=2) + "\n", encoding="utf8")
    (KIT / "diversity-report.json").write_text(json.dumps(diversity, indent=2) + "\n", encoding="utf8")

    status_path = KIT / "status.json"
    if status_path.is_file():
        status = json.loads(status_path.read_text(encoding="utf8"))
        status["release_ready"] = False
        status["human_listening_approved"] = False
        status["release_approved"] = False
        status["technical_status"] = qa.get("technical_status")
        status["diversity_status"] = diversity.get("status")
        status["decay_fix"] = "RATE-style longer duration/decay (memphis-tape ×1.12); HITL pending"
        status_path.write_text(json.dumps(status, indent=2) + "\n", encoding="utf8")

    return {
        "updated_recipes": updated,
        "rendered": rendered,
        "technical_qa": qa.get("technical_status"),
        "diversity": diversity.get("status"),
        "errors": qa.get("errors"),
        "release_ready": False,
    }


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("--renderer", type=Path, default=ROOT / "native" / "build" / "drumsmith-render")
    p.add_argument("--listen-only", action="store_true")
    a = p.parse_args()
    result = regen(a.renderer.resolve(strict=True), listen_only=a.listen_only)
    print(json.dumps(result, indent=2))
    sys.exit(0 if result["technical_qa"] == "PASS" and result["diversity"] == "PASS" else 1)
