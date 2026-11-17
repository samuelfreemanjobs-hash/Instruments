#!/usr/bin/env python3
"""DRUMSMITH script contract tests (19 cases). Exit 0 = package math/QA core OK."""

from __future__ import annotations

import subprocess
import sys
import tempfile
import wave
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))

from dsp_math import (  # noqa: E402
    bar_length_samples,
    beat_length_samples,
    dbfs_to_linear,
    glide_samples_for_pitch_drop,
    int24_full_scale,
    linear_to_dbfs,
    ms_to_samples,
    samples_to_ms,
)
from verify_kit import verify_file, verify_kit_dir  # noqa: E402

TESTS: list[tuple[str, bool]] = []


def check(name: str, cond: bool) -> None:
    TESTS.append((name, cond))
    if not cond:
        print(f"FAIL: {name}", file=sys.stderr)


def write_mono24_wav(path: Path, samples: list[int], rate: int = 44100) -> None:
    with wave.open(str(path), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(3)
        wf.setframerate(rate)
        frames = bytearray()
        for s in samples:
            if s < 0:
                s += 1 << 24
            frames.extend(bytes((s & 0xFF, (s >> 8) & 0xFF, (s >> 16) & 0xFF)))
        wf.writeframes(bytes(frames))


def main() -> int:
    check("bar_74bpm_44100", bar_length_samples(44100, 74) == 143027)
    check("bar_120bpm_48000", bar_length_samples(48000, 120) == 96000)
    check("bar_rejects_zero_bpm", _raises(lambda: bar_length_samples(44100, 0)))
    check("beat_90bpm", beat_length_samples(44100, 90) == 29400)
    check("ms_to_samples", ms_to_samples(10.0, 44100) == 441)
    check("samples_to_ms", abs(samples_to_ms(441, 44100) - 10.0) < 0.01)
    check("dbfs_neg6", abs(dbfs_to_linear(-6.0) - 0.5011872336272722) < 1e-6)
    check("linear_dbfs_neg_inf", linear_to_dbfs(0.0) == float("-inf"))
    check("glide_samples", glide_samples_for_pitch_drop(44100, 55.0, 40.0, 120.0) == 5292)
    check("int24_fs", int24_full_scale() == 8388607)

    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        quiet = int(0.25 * int24_full_scale())
        fade = [int(quiet * i / 8) for i in range(8)]
        body = [quiet] * (143027 - 16)
        samples = fade + body + list(reversed(fade))
        wav = tmp_path / "kick.wav"
        write_mono24_wav(wav, samples)
        res = verify_file(wav, 44100, 143027)
        check("verify_good_wav", res.ok and not res.fails)

        hot = int(0.95 * int24_full_scale())
        write_mono24_wav(tmp_path / "hot.wav", [hot] * 1000)
        res_hot = verify_file(tmp_path / "hot.wav", 44100, None)
        check("verify_peak_fail", not res_hot.ok)

        kit = tmp_path / "kit"
        kit.mkdir()
        write_mono24_wav(kit / "a.wav", samples)
        write_mono24_wav(kit / "b.wav", samples)
        summary = verify_kit_dir(kit, bpm=74.0)
        check("verify_kit_dir_ready", summary.ready)

        empty = tmp_path / "empty_kit"
        empty.mkdir()
        empty_summary = verify_kit_dir(empty)
        check("verify_empty_kit_fails", not empty_summary.ready)

    for name in ("dsp_math.py", "verify_kit.py", "self_test.py"):
        r = subprocess.run([sys.executable, "-m", "py_compile", str(SCRIPT_DIR / name)], capture_output=True)
        check(f"py_compile_{name}", r.returncode == 0)

    r_usage = subprocess.run([sys.executable, str(SCRIPT_DIR / "verify_kit.py")], capture_output=True)
    check("verify_cli_usage", r_usage.returncode == 2 and b"usage" in r_usage.stderr)

    r_child = subprocess.run([sys.executable, str(SCRIPT_DIR / "self_test.py"), "--child"], capture_output=True)
    check("child_exit_zero", r_child.returncode == 0)

    if len(TESTS) != 19:
        print(f"FAIL: expected 19 tests, got {len(TESTS)}", file=sys.stderr)
        return 1
    passed = sum(1 for _, ok in TESTS if ok)
    failed = len(TESTS) - passed
    print(f"self_test: {passed} passed, {failed} failed")
    return 0 if failed == 0 else 1


def _raises(fn) -> bool:
    try:
        fn()
        return False
    except ValueError:
        return True


if __name__ == "__main__":
    if "--child" in sys.argv:
        raise SystemExit(0)
    raise SystemExit(main())