"""Tests for studio WAV → factory kit packaging."""

from pathlib import Path

import numpy as np
import pytest
import soundfile as sf

import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from kit_from_samples import (  # noqa: E402
    build_kit_from_samples,
    resolve_slot_sources,
)

SR = 44100


def _write_tone(path: Path, freq: float) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    t = np.linspace(0, 0.1, int(SR * 0.1), endpoint=False, dtype=np.float32)
    sf.write(path, np.sin(2 * np.pi * freq * t), SR)


def test_resolve_slots_by_name(tmp_path: Path):
    _write_tone(tmp_path / "kick.wav", 80)
    _write_tone(tmp_path / "snare.wav", 200)
    resolved = resolve_slot_sources(tmp_path)
    names = {r.name for r in resolved}
    assert "kick" in names
    assert "snare" in names


def test_build_kit_manifest(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    stub_root = tmp_path / "stubs"
    for slot in ("kick", "snare", "hat_closed", "hat_open", "rim", "clap"):
        _write_tone(stub_root / "mpc-ready-808" / f"{slot}.wav", 100)
    samples = tmp_path / "samples"
    samples.mkdir()
    _write_tone(samples / "kick.wav", 60)
    _write_tone(samples / "snare.wav", 180)

    out = tmp_path / "kit"
    manifest_path = build_kit_from_samples(
        preset_id="mpc-ready-808",
        prompt="memphis phonk vae kit",
        samples_dir=tmp_path / "samples",
        out_dir=out,
        stub_root=stub_root,
    )
    assert manifest_path.is_file()
    manifest = manifest_path.read_text(encoding="utf-8")
    assert "DISKLORDZ_DRUM_KIT_MANIFEST" in manifest
    assert (out / "kick.wav").is_file()
    assert (out / "hat_closed.wav").is_file()
