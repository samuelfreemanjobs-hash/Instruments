#!/usr/bin/env python3
"""Audio QA gates v2 for factory outputs."""

from __future__ import annotations

import argparse
import json
import math
from dataclasses import asdict, dataclass
from pathlib import Path

from wav_util import read_mono_wav


@dataclass
class QaResult:
    path: str
    pass_qa: bool
    peak: float
    rms: float
    crest_factor: float
    clipped_samples: int
    duration_s: float
    spectral_slope: float
    low_band_ratio: float
    reasons: list[str]


def spectral_slope(samples: list[float], rate: int) -> float:
    """High-frequency vs mid energy ratio (higher = harsher)."""
    if len(samples) < rate // 4:
        return 0.0
    window = min(len(samples), rate)
    seg = samples[:window]
    diffs = [abs(seg[i] - seg[i - 1]) for i in range(1, len(seg))]
    return sum(diffs) / max(len(diffs), 1)


def low_band_ratio(samples: list[float]) -> float:
    state = 0.0
    alpha = 0.015
    low_e = total_e = 0.0
    for s in samples:
        state += alpha * (s - state)
        low_e += state * state
        total_e += s * s
    return low_e / (total_e + 1e-12)


def analyze_wav(path: Path, max_slope: float = 0.35, max_crest: float = 18.0) -> QaResult:
    samples, rate = read_mono_wav(path)
    reasons: list[str] = []
    if not samples:
        return QaResult(str(path), False, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, ["empty_file"])

    peak = max(abs(s) for s in samples)
    rms = math.sqrt(sum(s * s for s in samples) / len(samples))
    crest = (peak / rms) if rms > 1e-9 else 0.0
    clipped = sum(1 for s in samples if abs(s) >= 0.999)
    duration = len(samples) / rate
    slope = spectral_slope(samples, rate)
    low_ratio = low_band_ratio(samples)

    if peak < 0.02:
        reasons.append("too_quiet")
    if rms < 0.005:
        reasons.append("near_silence")
    if clipped > len(samples) * 0.001:
        reasons.append("clipping")
    if duration < 0.08:
        reasons.append("too_short")
    if duration > 15.0:
        reasons.append("too_long")
    if crest > max_crest:
        reasons.append("harsh_crest")
    if slope > max_slope:
        reasons.append("harsh_slope")
    if low_ratio > 0.995 and rms < 0.04:
        reasons.append("mono_sub_only")

    return QaResult(
        str(path),
        pass_qa=len(reasons) == 0,
        peak=peak,
        rms=rms,
        crest_factor=crest,
        clipped_samples=clipped,
        duration_s=duration,
        spectral_slope=slope,
        low_band_ratio=low_ratio,
        reasons=reasons,
    )


def qa_score(r: QaResult) -> float:
    """Higher is better for best-of-N."""
    if not r.pass_qa:
        return -1.0
    return r.rms * (1.0 - min(r.spectral_slope, 0.5))


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("wavs", nargs="+", type=Path)
    p.add_argument("--json-out", type=Path)
    p.add_argument("--fail-fast", action="store_true")
    args = p.parse_args()

    results = [analyze_wav(w) for w in args.wavs]
    ok = all(r.pass_qa for r in results)
    payload = {"pass": ok, "version": 2, "results": [asdict(r) for r in results]}
    text = json.dumps(payload, indent=2)
    print(text)
    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(text, encoding="utf-8")
    if args.fail_fast and not ok:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
