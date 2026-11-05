#!/usr/bin/env python3
"""
Start here: style brief → 100 one-shot WAVs from any VST3 factory bank.

Pipeline (three roles, one command):
  1. Recipe Author  — 100 slots from your style text
  2. Preset Matcher — factory program per slot
  3. Render Executor — VstiOneShotRender (no musical judgment)

Example:
  python3 tools/vsti-one-shot/boom.py \\
    --style "dark Memphis melodic stabs and warm keys" \\
    --vst3 "/Library/Audio/Plug-Ins/VST3/Serum.vst3" \\
    --out ~/Exports/mpc_melody_pack
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_RENDER_BIN = REPO_ROOT / "build" / "VstiOneShotRender"

# Allow running without installing the package
sys.path.insert(0, str(Path(__file__).resolve().parent))

from vsti_one_shot.matcher import (  # noqa: E402
    build_render_job,
    match_assignments,
    match_assignments_offline,
)
from vsti_one_shot.recipe_author import SLOT_COUNT, build_recipe  # noqa: E402


def main() -> int:
    p = argparse.ArgumentParser(description="Style → 100 VST3 one-shot WAVs for MPC")
    p.add_argument("--style", required=True, help="Natural language style brief")
    p.add_argument("--vst3", required=True, type=Path, help="Path to .vst3 bundle")
    p.add_argument("--out", required=True, type=Path, help="Output directory for WAVs")
    p.add_argument(
        "--render-bin",
        type=Path,
        default=DEFAULT_RENDER_BIN,
        help="Path to VstiOneShotRender binary",
    )
    p.add_argument(
        "--count",
        type=int,
        default=SLOT_COUNT,
        help=f"Number of one-shots (default {SLOT_COUNT})",
    )
    p.add_argument(
        "--dry-run",
        action="store_true",
        help="Write recipe/assignments/job only; do not render",
    )
    p.add_argument(
        "--match-mode",
        choices=("names", "preview", "hybrid"),
        default="hybrid",
        help="Preset matcher: name heuristics, preview audio, or both (default hybrid)",
    )
    args = p.parse_args()

    if not args.vst3.exists():
        print(f"ERROR: VST3 not found: {args.vst3}", file=sys.stderr)
        return 1
    if not args.dry_run and not args.render_bin.is_file():
        print(
            f"ERROR: VstiOneShotRender not found at {args.render_bin}\n"
            "Build: cmake --build build -j --target VstiOneShotRender",
            file=sys.stderr,
        )
        return 1
    if args.dry_run and not args.render_bin.is_file():
        print("Dry run: render binary missing — using offline matcher.", file=sys.stderr)

    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    work_dir = args.out / f".boom_{stamp}"
    work_dir.mkdir(parents=True, exist_ok=True)
    args.out.mkdir(parents=True, exist_ok=True)

    recipe = build_recipe(args.style, slot_count=args.count)
    recipe_path = work_dir / "recipe.json"
    recipe_path.write_text(json.dumps(recipe, indent=2), encoding="utf-8")

    if args.dry_run and not args.render_bin.is_file():
        assignments = match_assignments_offline(recipe, args.vst3)
    else:
        assignments = match_assignments(
            recipe,
            args.vst3,
            args.render_bin,
            match_mode=args.match_mode,
            work_dir=work_dir,
        )
    assignments_path = work_dir / "assignments.json"
    assignments_path.write_text(json.dumps(assignments, indent=2), encoding="utf-8")

    job = build_render_job(recipe, assignments, args.out)
    job_path = work_dir / "job.json"
    job_path.write_text(json.dumps(job, indent=2), encoding="utf-8")

    print(f"Recipe Author: {len(recipe['slots'])} slots → {recipe_path}")
    mode = assignments.get("match_mode", args.match_mode)
    print(
        f"Preset Matcher ({mode}): {len(assignments['assignments'])} programs → {assignments_path}"
    )
    print(f"Render job: {job_path}")

    if args.dry_run:
        print("Dry run — skipping render.")
        return 0

    proc = subprocess.run(
        [str(args.render_bin), "--job", str(job_path)],
        check=False,
    )
    if proc.returncode != 0:
        return proc.returncode

    manifest = args.out / "manifest.json"
    print(f"Boom — WAVs in {args.out} (manifest: {manifest})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
