#!/usr/bin/env python3
"""CLI entry: run the full Instruments QA pipeline (CI parity + optional SaaS)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import replace
from pathlib import Path

OPS = Path(__file__).resolve().parent
sys.path.insert(0, str(OPS))

from business_pipeline import PROFILES, PipelineConfig, run_pipeline  # noqa: E402


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run monorepo business / QA pipeline")
    parser.add_argument(
        "--profile",
        choices=list(PROFILES.keys()),
        default="ci",
        help="ci=GitHub build.yml parity; full=+ Disklordz web; plugin-quick=pluginval only",
    )
    parser.add_argument("--no-stop-on-fail", action="store_true")
    parser.add_argument("--with-saas", action="store_true", help="Include Disklordz npm build (full profile)")
    args = parser.parse_args(argv)

    cfg = PROFILES[args.profile]
    if args.with_saas:
        cfg = replace(cfg, disklordz_web=True)

    run = run_pipeline(args.profile, cfg, stop_on_fail=not args.no_stop_on_fail)
    print(json.dumps(run.to_dict(), indent=2))
    for stage in run.stages:
        mark = "PASS" if stage.ok else "FAIL"
        print(f"[{mark}] {stage.stage.value} ({stage.duration_s:.1f}s)")
    return 0 if run.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
