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
from trap_lord_audio_metrics import (  # noqa: E402
    CROSS_CATEGORY_CORR_MAX,
    analyze_file,
    assert_wav_format,
    category_spectral_passes,
    correlation,
    mpc_program_peak_ok,
)

FX_CATEGORIES = {"Hits/FX"}
NON_FX_CENTROID_MAX_HZ = 6000.0
PEAK_MAX = 0.99
TAIL_MAX_DBFS = -60.0
FUNDAMENTAL_TARGET_HZ = 261.63
FUNDAMENTAL_TOLERANCE = 8.0


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


def pick_audible_preset_index(category: str, tmp: Path, max_try: int = 12) -> int:
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    tried = 0
    for i, p in enumerate(bank["presets"]):
        if p["category"] != category:
            continue
        if tried >= max_try:
            break
        tried += 1
        wav = tmp / f"probe_{category.replace('/', '_')}_{i}.wav"
        json_path = tmp / f"probe_{i}.json"
        json_path.write_text(json.dumps(p), encoding="utf-8")
        subprocess.check_call([str(CLI), "category-probe-json", str(json_path), str(wav)], cwd=ROOT)
        metrics = analyze_file(wav, category=category)
        if metrics.peak > 0.02:
            return i
    raise AssertionError(f"no audible preset found for category {category}")


def assert_preset_diversity() -> None:
    tmp = ROOT / "build" / "audio_qa_presets"
    tmp.mkdir(parents=True, exist_ok=True)
    pad_ix = pick_audible_preset_index("Pads", tmp)
    bass_ix = pick_audible_preset_index("Bass", tmp)
    pad_wav = tmp / "pad.wav"
    bass_wav = tmp / "bass.wav"
    subprocess.check_call([str(CLI), "audition", str(pad_ix), str(pad_wav)], cwd=ROOT)
    subprocess.check_call([str(CLI), "audition", str(bass_ix), str(bass_wav)], cwd=ROOT)

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
        ok, reason = category_spectral_passes(m, m.category)
        assert ok, f"{m.path} failed category spectral gate: {reason}"


def assert_mpc_program_normalize() -> None:
    tmp = ROOT / "build" / "audio_qa_mpc"
    tmp.mkdir(parents=True, exist_ok=True)
    bank = json.loads(BANK.read_text(encoding="utf-8"))
    pad_ix = next(i for i, p in enumerate(bank["presets"]) if p["category"] == "Pads")
    out_dir = tmp / "pad_program"
    subprocess.check_call([str(CLI), "mpc", str(pad_ix), str(out_dir), "1"], cwd=ROOT)
    ok, peak, bits = mpc_program_peak_ok(out_dir)
    assert bits == 3, f"MPC WAV bit depth {bits}, expected 24-bit"
    assert ok, f"MPC program peak {peak:.4f} outside normalized band"


def assert_cross_category_gate_script() -> None:
    script = TOOLS / "verify_cross_category_gate.py"
    subprocess.check_call([sys.executable, str(script)], cwd=ROOT)


def test_audition_wav_contract(stage_wav: Path | None = None) -> None:
    """If staging WAV exists from package run, assert 44.1 kHz / 24-bit."""
    path = stage_wav or (ROOT / "build" / "audio_qa_presets" / "pad.wav")
    if path.exists():
        assert_wav_format(path, sample_rate=44100, sample_width_bytes=3)


def test_trap_lord_audio_qa() -> None:
    build_cli()
    assert_diagnose()
    assert_preset_diversity()
    assert_mpc_program_normalize()
    assert_cross_category_gate_script()


if __name__ == "__main__":
    test_trap_lord_audio_qa()
    print("trap_lord_audio_qa: OK")
