"""Binary compilation bridge (external serum-preset-packager / PySerum)."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
from pathlib import Path

from serum_forge.models import SerumSymbolPatch


class CompilerNotConfiguredError(RuntimeError):
    pass


def write_intermediate_json(patch: SerumSymbolPatch, path: Path) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(patch.to_intermediate_json(), indent=2), encoding="utf-8")
    return path


def compile_serum_preset(
    patch: SerumSymbolPatch,
    out_path: Path,
    *,
    packager_bin: str | None = None,
) -> Path:
    """Emit .SerumPreset when SERUM_PACKAGER_BIN or packager_bin is set.

    Expected CLI (configure to match your local serum-preset-packager install):
      serum-preset-packager build --input preset.json --output Patch.SerumPreset
    """
    bin_path = packager_bin or os.environ.get("SERUM_PACKAGER_BIN", "")
    if not bin_path or not shutil.which(bin_path.split()[0] if " " in bin_path else bin_path):
        raise CompilerNotConfiguredError(
            "Set SERUM_PACKAGER_BIN to your packager executable. "
            "Intermediate JSON was validated; binary pack skipped."
        )

    tmp_json = out_path.with_suffix(".json")
    write_intermediate_json(patch, tmp_json)
    cmd = os.environ.get(
        "SERUM_PACKAGER_CMD",
        "{bin} build --input {infile} --output {outfile}",
    ).format(bin=bin_path, infile=tmp_json, outfile=out_path)
    subprocess.run(cmd, shell=True, check=True, capture_output=True, text=True)
    return out_path
