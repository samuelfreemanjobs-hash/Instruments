#!/usr/bin/env python3
"""Build headless CLI, render 24 audition WAVs, normalize, QA, metrics, spectrograms, zip."""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
CLI = BUILD / "TrapLordSynthRenderCLI_artefacts" / "Release" / "TrapLordSynthRenderCLI"
TOOLS = ROOT / "tools"
sys.path.insert(0, str(TOOLS))

from trap_lord_audio_metrics import analyze_file, assert_wav_format, write_metrics_csv  # noqa: E402

ARTIFACTS = Path("/opt/cursor/artifacts")
AUDITION_ZIP = ARTIFACTS / "trap-lord-synth-auditions-24.zip"
CSV_OUT = ARTIFACTS / "trap-lord-synth-auditions-metrics.csv"
MANIFEST = ROOT / "assets/factory/audition_manifest.json"
BANK = ROOT / "assets/factory/preset_bank.json"

PEAK_MAX = 0.99
TAIL_MAX_DBFS = -60.0
CENTROID_MAX_HZ = 6000.0
FX_CATEGORIES = {"Hits/FX"}
DC_MAX = 0.01


def run(cmd: list[str], **kwargs) -> None:
    print("+", " ".join(cmd))
    subprocess.check_call(cmd, **kwargs)


def cmake_build_cli() -> None:
    if not BUILD.exists():
        run(
            [
                "cmake",
                "-B",
                str(BUILD),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ],
            cwd=ROOT,
        )
    run(["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynthRenderCLI"], cwd=ROOT)
    if not CLI.exists():
        raise SystemExit(f"CLI missing: {CLI}")


def normalize_lufs(path: Path, target_lufs: float = -14.0, tp_db: float = -1.0) -> None:
    tmp = path.with_suffix(".norm.wav")
    filt = f"highpass=f=25,loudnorm=I={target_lufs}:TP={tp_db}:LRA=11:linear=true"
    try:
        run(
            [
                "ffmpeg",
                "-y",
                "-hide_banner",
                "-loglevel",
                "error",
                "-i",
                str(path),
                "-af",
                filt,
                "-ar",
                "44100",
                "-c:a",
                "pcm_s24le",
                str(tmp),
            ]
        )
        shutil.move(tmp, path)
    except (FileNotFoundError, subprocess.CalledProcessError):
        from trap_lord_audio_metrics import read_wav_mono

        x, sr = read_wav_mono(path)
        rms = float((x**2).mean() ** 0.5)
        if rms > 1e-8:
            target_rms = 10 ** (target_lufs / 20.0) * 0.1
            gain = min(target_rms / rms, 8.0)
            peak = float(abs(x).max() * gain)
            if peak > 10 ** (tp_db / 20.0):
                gain *= 10 ** (tp_db / 20.0) / peak
            x = (x * gain).clip(-1.0, 1.0)
            import wave
            import struct

            with wave.open(str(path), "wb") as wf:
                wf.setnchannels(1)
                wf.setsampwidth(3)
                wf.setframerate(sr)
                frames = b"".join(
                    struct.pack("<i", int(max(-(1 << 23), min((1 << 23) - 1, int(s * (1 << 23))))))[:3]
                    for s in x
                )
                wf.writeframes(frames)


def category_from_filename(name: str) -> str:
    return name.split("_", 1)[0] if "_" in name else ""


def make_spectrogram(path: Path, out_png: Path) -> None:
    try:
        import matplotlib

        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        from trap_lord_audio_metrics import read_wav_mono
        import numpy as np

        x, sr = read_wav_mono(path)
        if len(x) < 256:
            return
        nperseg = 2048
        noverlap = nperseg // 2
        spec, freqs, times, im = plt.specgram(x, NFFT=nperseg, Fs=sr, noverlap=noverlap, cmap="magma")
        plt.xlabel("Time (s)")
        plt.ylabel("Hz")
        plt.title(path.stem)
        plt.colorbar(im, label="dB")
        out_png.parent.mkdir(parents=True, exist_ok=True)
        plt.savefig(out_png, dpi=120, bbox_inches="tight")
        plt.close()
    except ImportError:
        print("matplotlib not installed; skipping spectrogram", path.name)


def qa_file(metrics, wav_path: Path) -> None:
    assert_wav_format(wav_path, sample_rate=44100, sample_width_bytes=3)
    assert metrics.peak > 1e-4, f"{metrics.path}: silent render"
    assert metrics.peak <= PEAK_MAX, f"{metrics.path}: peak {metrics.peak}"
    assert abs(metrics.dc_offset) <= DC_MAX, f"{metrics.path}: dc {metrics.dc_offset}"
    # Full 8s phrase auditions intentionally retain short FX tail; strict tail gate lives in test_trap_lord_audio_qa (diagnose).
    if metrics.category and metrics.category not in FX_CATEGORIES:
        assert metrics.centroid_hz < CENTROID_MAX_HZ, (
            f"{metrics.path}: centroid {metrics.centroid_hz} Hz"
        )


def main() -> int:
    ARTIFACTS.mkdir(parents=True, exist_ok=True)
    stage = ARTIFACTS / "trap-lord-auditions-staging"
    if stage.exists() and not stage.is_dir():
        stage.unlink()
    shutil.rmtree(stage, ignore_errors=True)
    stage.mkdir(parents=True, exist_ok=True)

    cmake_build_cli()
    run([sys.executable, str(ROOT / "tests/test_trap_lord_audio_qa.py")], cwd=ROOT)

    run([sys.executable, str(TOOLS / "pick_distinct_auditions.py")], cwd=ROOT)
    run([str(CLI), "auditions", str(stage), str(MANIFEST)], cwd=ROOT)
    wavs = sorted(stage.glob("*.wav"))
    if len(wavs) != 24:
        raise SystemExit(f"Expected 24 WAV files, got {len(wavs)}")

    for w in wavs:
        normalize_lufs(w)

    metrics_rows = []
    for w in wavs:
        cat = category_from_filename(w.name)
        m = analyze_file(w, category=cat)
        qa_file(m, w)
        metrics_rows.append(m)

    write_metrics_csv(metrics_rows, CSV_OUT)

    bank = json.loads(BANK.read_text(encoding="utf-8"))
    by_cat: dict[str, list] = {}
    for i, p in enumerate(bank["presets"]):
        by_cat.setdefault(p["category"], []).append({"index": i, "name": p["name"]})
    auditions = []
    for cat in sorted(by_cat):
        for slot, pick in enumerate(by_cat[cat][:2], start=1):
            auditions.append({"category": cat, "slot": slot, **pick})
    MANIFEST.write_text(json.dumps({"auditions": auditions, "wavCount": 24}, indent=2), encoding="utf-8")

    def pick_prefix(prefix: str) -> Path:
        return next(w for w in wavs if w.name.startswith(prefix))

    picks = {
        "pad": pick_prefix("Pads_"),
        "lead": pick_prefix("Leads_"),
        "keys": pick_prefix("Synth_Keys_") if any(w.name.startswith("Synth_Keys_") for w in wavs) else pick_prefix("Synth "),
        "swell": pick_prefix("Swells_"),
    }
    spec_dir = ARTIFACTS / "trap-lord-spectrograms"
    shutil.rmtree(spec_dir, ignore_errors=True)
    spec_dir.mkdir(parents=True)
    for label, wav in picks.items():
        make_spectrogram(wav, spec_dir / f"{label}.png")

    with zipfile.ZipFile(AUDITION_ZIP, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for w in wavs:
            zf.write(w, w.name)
        zf.write(MANIFEST, "audition_manifest.json")
        zf.write(CSV_OUT, CSV_OUT.name)
        for png in spec_dir.glob("*.png"):
            zf.write(png, f"spectrograms/{png.name}")

    print(f"Wrote {AUDITION_ZIP}")
    print(f"Metrics: {CSV_OUT}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
