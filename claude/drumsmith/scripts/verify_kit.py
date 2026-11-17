#!/usr/bin/env python3
"""Read-only WAV kit QA for DRUMSMITH / CALIPER (stdlib only)."""

from __future__ import annotations

import json
import sys
import wave
from dataclasses import dataclass, field
from pathlib import Path

from dsp_math import bar_length_samples, linear_to_dbfs

PEAK_CEILING_DBFS = -1.0
DC_OFFSET_MAX = 0.02
MICRO_FADE_SAMPLES = 8
CLIP_THRESHOLD = 0.999


@dataclass
class CheckResult:
    path: str
    ok: bool
    warns: list[str] = field(default_factory=list)
    fails: list[str] = field(default_factory=list)


@dataclass
class KitSummary:
    files: list[CheckResult] = field(default_factory=list)
    global_fails: list[str] = field(default_factory=list)
    global_warns: list[str] = field(default_factory=list)

    @property
    def ready(self) -> bool:
        if self.global_fails:
            return False
        return all(not f.fails for f in self.files)


def _read_pcm24_mono(path: Path) -> tuple[list[int], int]:
    with wave.open(str(path), "rb") as wf:
        if wf.getnchannels() != 1:
            raise ValueError(f"{path.name}: expected mono")
        if wf.getsampwidth() != 3:
            raise ValueError(f"{path.name}: expected 24-bit PCM")
        rate = wf.getframerate()
        raw = wf.readframes(wf.getnframes())
    samples: list[int] = []
    full = 1 << 23
    for i in range(0, len(raw), 3):
        b0, b1, b2 = raw[i], raw[i + 1], raw[i + 2]
        v = b0 | (b1 << 8) | (b2 << 16)
        if v & 0x800000:
            v -= 1 << 24
        samples.append(v)
    return samples, rate


def _peak_linear(samples: list[int]) -> float:
    fs = (1 << 23) - 1
    peak = max(abs(s) for s in samples) if samples else 0
    return peak / fs


def _dc_offset(samples: list[int]) -> float:
    if not samples:
        return 0.0
    fs = float((1 << 23) - 1)
    return sum(samples) / (len(samples) * fs)


def _has_micro_fades(samples: list[int]) -> bool:
    if len(samples) < MICRO_FADE_SAMPLES * 2:
        return True
    fs = float((1 << 23) - 1)
    head = [abs(s) / fs for s in samples[:MICRO_FADE_SAMPLES]]
    tail = [abs(s) / fs for s in samples[-MICRO_FADE_SAMPLES:]]
    return head[0] < CLIP_THRESHOLD and tail[-1] < CLIP_THRESHOLD


def _clipping(samples: list[int]) -> bool:
    fs = (1 << 23) - 1
    return any(abs(s) >= int(fs * CLIP_THRESHOLD) for s in samples)


def verify_file(path: Path, expected_rate: int | None, bar_samples: int | None) -> CheckResult:
    res = CheckResult(path=str(path), ok=True)
    try:
        samples, rate = _read_pcm24_mono(path)
    except Exception as exc:  # noqa: BLE001
        res.fails.append(str(exc))
        res.ok = False
        return res

    if expected_rate is not None and rate != expected_rate:
        res.fails.append(f"sample rate {rate} != kit {expected_rate}")

    peak = _peak_linear(samples)
    peak_db = linear_to_dbfs(peak)
    if peak_db > PEAK_CEILING_DBFS:
        res.fails.append(f"peak {peak_db:.2f} dBFS exceeds ceiling {PEAK_CEILING_DBFS} dBFS")
    if _clipping(samples):
        res.fails.append("clipping detected at ≥99.9% full scale")

    dc = _dc_offset(samples)
    if abs(dc) > DC_OFFSET_MAX:
        res.warns.append(f"DC offset {dc:.4f} exceeds warn threshold {DC_OFFSET_MAX}")

    if not _has_micro_fades(samples):
        res.warns.append("missing micro-fade at file head/tail")

    if bar_samples is not None and len(samples) % bar_samples != 0:
        res.warns.append(
            f"length {len(samples)} samples not on bar grid ({bar_samples} samples/bar)"
        )

    if res.fails:
        res.ok = False
    return res


def verify_kit_dir(
    kit_dir: Path,
    *,
    expected_rate: int = 44100,
    bpm: float | None = None,
    beats_per_bar: int = 4,
) -> KitSummary:
    wavs = sorted(kit_dir.glob("**/*.wav"))
    summary = KitSummary()
    if not wavs:
        summary.global_fails.append(f"no WAV files under {kit_dir}")
        return summary

    bar_n: int | None = None
    if bpm is not None:
        bar_n = bar_length_samples(expected_rate, bpm, beats_per_bar)

    rates: set[int] = set()
    for wav in wavs:
        try:
            with wave.open(str(wav), "rb") as wf:
                rates.add(wf.getframerate())
        except wave.Error as exc:
            summary.global_fails.append(f"{wav.name}: {exc}")

    if len(rates) > 1:
        summary.global_fails.append(f"mixed sample rates: {sorted(rates)}")
    elif rates:
        only = next(iter(rates))
        if only != expected_rate:
            summary.global_warns.append(f"kit rate {only} != expected {expected_rate}")

    exp_rate = expected_rate if len(rates) <= 1 else None
    for wav in wavs:
        summary.files.append(verify_file(wav, exp_rate, bar_n))
    return summary


def main(argv: list[str] | None = None) -> int:
    argv = argv if argv is not None else sys.argv[1:]
    if not argv:
        print("usage: verify_kit.py <kit_dir> [--bpm BPM] [--json]", file=sys.stderr)
        return 2
    kit_dir = Path(argv[0])
    bpm: float | None = None
    as_json = False
    i = 1
    while i < len(argv):
        if argv[i] == "--bpm" and i + 1 < len(argv):
            bpm = float(argv[i + 1])
            i += 2
        elif argv[i] == "--json":
            as_json = True
            i += 1
        else:
            i += 1

    summary = verify_kit_dir(kit_dir, bpm=bpm)
    if as_json:
        payload = {
            "ready": summary.ready,
            "global_fails": summary.global_fails,
            "global_warns": summary.global_warns,
            "files": [
                {
                    "path": f.path,
                    "ok": f.ok,
                    "warns": f.warns,
                    "fails": f.fails,
                }
                for f in summary.files
            ],
        }
        print(json.dumps(payload, indent=2))
    else:
        for f in summary.files:
            status = "PASS" if f.ok and not f.fails else "FAIL"
            print(f"{status} {f.path}")
            for w in f.warns:
                print(f"  WARN {w}")
            for fl in f.fails:
                print(f"  FAIL {fl}")
        for g in summary.global_warns:
            print(f"WARN {g}")
        for g in summary.global_fails:
            print(f"FAIL {g}")
        print(f"READY={'yes' if summary.ready else 'no'}")
    return 0 if summary.ready else 1


if __name__ == "__main__":
    raise SystemExit(main())