"""Shared category-probe-json helpers for factory bank tooling."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"


def probe_wav_path(probe_dir: Path, index: int) -> Path:
    return probe_dir / f"p_{index:04d}.wav"


def probe_json_path(probe_dir: Path, index: int) -> Path:
    return probe_dir / "json" / f"p_{index:04d}.json"


def render_category_probe(preset: dict, index: int, probe_dir: Path) -> Path:
    wav = probe_wav_path(probe_dir, index)
    json_path = probe_json_path(probe_dir, index)
    json_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(preset), encoding="utf-8")
    subprocess.check_call([str(CLI), "category-probe-json", str(json_path), str(wav)], cwd=ROOT)
    return wav
