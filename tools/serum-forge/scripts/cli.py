#!/usr/bin/env python3
"""Serum Forge CLI — intent → symbolic preset → optional binary."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from serum_forge.models import PatchArchetype  # noqa: E402
from serum_forge.param_spec import param_openapi_schema  # noqa: E402
from serum_forge.pipeline import run_pipeline  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description="Serum Forge autonomous preset pipeline")
    parser.add_argument("--prompt", required=True, help="Natural language sound brief")
    parser.add_argument(
        "--archetype",
        choices=[a.value for a in PatchArchetype],
        default=PatchArchetype.pad_ambient.value,
    )
    parser.add_argument("--out-dir", type=Path, default=Path("out/serum-forge"))
    parser.add_argument("--compile", action="store_true", help="Invoke SERUM_PACKAGER_BIN")
    parser.add_argument("--schema", action="store_true", help="Print OpenAPI JSON schema and exit")
    args = parser.parse_args()

    if args.schema:
        print(json.dumps(param_openapi_schema(), indent=2))
        return 0

    result = run_pipeline(
        args.prompt,
        archetype=PatchArchetype(args.archetype),
        out_dir=args.out_dir,
        compile_binary=args.compile,
    )
    payload = {
        "name": result.patch.name,
        "archetype": result.patch.archetype.value,
        "json": str(result.intermediate_json) if result.intermediate_json else None,
        "serum_preset": str(result.serum_preset) if result.serum_preset else None,
        "warnings": result.warnings,
    }
    print(json.dumps(payload, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
