#!/usr/bin/env python3
"""CI audio QA gate for Trap Lord Synth headless renders."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
CLI = ROOT / "build" / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
BANK = ROOT / "assets" / "factory" / "preset_bank.json"

sys.path.insert(0, str(TOOLS))
from trap_lord_audio_metrics import analyze_file, assert_wav_format, correlation  # noqa: E402

FX_CATEGORIES = {"Hits/FX"}
NON_FX_CENTROID_MAX_HZ = 6000.0
PEAK_MAX = 0.99
TAIL_MAX_DBFS = -60.0
FUNDAMENTAL_TARGET_HZ = 261.63
FUNDAMENTAL_TOLERANCE = 8.0
CROSS_CATEGORY_CORR_MAX = 0.9


def build_cli() -> None:
    build = ROOT / "build"
    if not build.exists():
        subprocess.check_call(
            [
                "cmake",
                "-B",
                str(build),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ],
            cwd=ROOT,
        )
    subprocess.check_call(
        ["cmake", "--build", str(build), "-j", "--target", "TrapLordSynthRenderCLI"], cwd=ROOT
    )
    if not CLI.exists():
        raise AssertionError(f"missing CLI: {CLI}")


def assert_diagnose() -> None:
    diag_dir = ROOT / "build" / "audio_qa_diagnose"
    diag_dir.mkdir(parents=True, exist_ok=True)
    subprocess.check_call([str(CLI), "diagnose", str(diag_dir)], cwd=ROOT)

    for name in ("init_saw.wav", "sine.wav", "middle_c.wav"):
        assert_wav_format(diag_dir / name, sample_rate=44100, sample_width_bytes=3)

    mid_path = diag_dir / "middle_c.wav"
    mid = analyze_file(mid_path)
    assert mid.peak > 0.02, "middle_c render is near-silent"
    assert mid.peak <= PEAK_MAX, f"middle_c peak {mid.peak} exceeds {PEAK_MAX}"
    assert abs(mid.fundamental_hz - FUNDAMENTAL_TARGET_HZ) <= FUNDAMENTAL_TOLERANCE, (
        f"expected fundamental near {FUNDAMENTAL_TARGET_HZ} Hz, got {mid.fundamental_hz}"
    )
    assert mid.tail_rms_db <= TAIL_MAX_DBFS, f"release tail too loud: {mid.tail_rms_db} dBFS"

    saw = analyze_file(diag_dir / "init_saw.wav")
    assert saw.centroid_hz < NON_FX_CENTROID_MAX_HZ, f"init saw centroid too bright: {saw.centroid_hz}"


def pick_presets_by_category() -> dict[str, int]:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    out: dict[str, int] = {}
    for i, p in enumerate(bank["presets"]):
        cat = p["category"]
        if cat not in out:
            out[cat] = i
    return out


def assert_preset_diversity() -> None:
    picks = pick_presets_by_category()
    assert "Pads" in picks and "Bass" in picks
    tmp = ROOT / "build" / "audio_qa_presets"
    tmp.mkdir(parents=True, exist_ok=True)
    pad_wav = tmp / "pad.wav"
    bass_wav = tmp / "bass.wav"
    subprocess.check_call([str(CLI), "audition", str(picks["Pads"]), str(pad_wav)], cwd=ROOT)
    subprocess.check_call([str(CLI), "audition", str(picks["Bass"]), str(bass_wav)], cwd=ROOT)

    pad = analyze_file(pad_wav, category="Pads")
    bass = analyze_file(bass_wav, category="Bass")
    assert pad.peak > 0.02 and bass.peak > 0.02, "preset audition renders are silent"
    corr = correlation(pad_wav, bass_wav)
    assert corr < CROSS_CATEGORY_CORR_MAX, f"Pads vs Bass too similar (corr={corr:.3f})"

    for m in (pad, bass):
        assert m.peak <= PEAK_MAX
        if m.category not in FX_CATEGORIES:
            assert m.centroid_hz < NON_FX_CENTROID_MAX_HZ, (
                f"{m.path} centroid {m.centroid_hz} Hz > {NON_FX_CENTROID_MAX_HZ}"
            )
        assert abs(m.dc_offset) < 0.02, f"{m.path} dc offset {m.dc_offset}"


def test_audition_wav_contract(stage_wav: Path | None = None) -> None:
    """If staging WAV exists from package run, assert 44.1 kHz / 24-bit."""
    path = stage_wav or (ROOT / "build" / "audio_qa_presets" / "pad.wav")
    if path.exists():
        assert_wav_format(path, sample_rate=44100, sample_width_bytes=3)


def test_trap_lord_audio_qa() -> None:
    build_cli()
    assert_diagnose()
    assert_preset_diversity()


if __name__ == "__main__":
    test_trap_lord_audio_qa()
    print("trap_lord_audio_qa: OK")
