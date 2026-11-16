#!/usr/bin/env python3
"""Report DC, crest factor, an inter-sample peak estimate, and sub-bass mono coherence."""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from pcmio import read_wav_channels


def lowpass(samples: list[float], rate: int, cutoff: float = 120.0) -> list[float]:
    if rate <= 0:
        return list(samples)
    coefficient = math.exp(-2.0 * math.pi * cutoff / rate)
    state = 0.0
    output: list[float] = []
    for sample in samples:
        state = (1.0 - coefficient) * sample + coefficient * state
        output.append(state)
    return output


def interpolate_peak(samples: list[float]) -> float:
    """4x linear estimate. This is not an ITU-R BS.1770 true-peak meter."""
    if not samples:
        return 0.0
    peak = 0.0
    for index, sample in enumerate(samples[:-1]):
        nxt = samples[index + 1]
        for step in range(4):
            mix = sample + (nxt - sample) * (step / 4.0)
            peak = max(peak, abs(mix))
    return max(peak, abs(samples[-1]))


def correlation(left: list[float], right: list[float]) -> float:
    count = min(len(left), len(right))
    if count == 0:
        return 1.0
    mean_left = sum(left[:count]) / count
    mean_right = sum(right[:count]) / count
    num = den_left = den_right = 0.0
    for index in range(count):
        a = left[index] - mean_left
        b = right[index] - mean_right
        num += a * b
        den_left += a * a
        den_right += b * b
    den = math.sqrt(den_left * den_right)
    if den < 1.0e-12:
        return 1.0
    return num / den


def channel_coherence(channels: list[list[float]], rate: int) -> tuple[float, float]:
    """Lowest sub-bass correlation between any two original channels."""
    if len(channels) < 2:
        return 1.0, 0.0
    filtered = [lowpass(channel, rate) for channel in channels]
    energy = 0.0
    coherence = 1.0
    for index, left in enumerate(filtered):
        energy = max(energy, sum(sample * sample for sample in left) / len(left))
        for right in filtered[index + 1 :]:
            coherence = min(coherence, correlation(left, right))
    return coherence, energy


def audit(path: Path) -> dict:
    separated, rate = read_wav_channels(path)
    if not separated or not separated[0]:
        return {"file": path.name, "passed": False, "problems": ["empty"]}
    channels = len(separated)
    samples = [
        sum(channel[index] for channel in separated) / channels
        for index in range(len(separated[0]))
    ]
    mean = sum(samples) / len(samples)
    peak = max(abs(sample) for sample in samples)
    rms = math.sqrt(sum(sample * sample for sample in samples) / len(samples))
    crest = peak / rms if rms > 1.0e-9 else 0.0
    intersample = interpolate_peak(samples)
    coherence, sub_energy = channel_coherence(separated, rate)
    problems: list[str] = []
    if abs(mean) > 0.02:
        problems.append("dc offset")
    if intersample > 1.0:
        problems.append("intersample peak above 0 dBTP estimate")
    if not math.isfinite(peak):
        problems.append("non-finite")
    punch_warning = crest < 4.0 and any(token in path.name.lower() for token in ("808", "kick"))
    if sub_energy < 1.0e-6:
        coherence = 1.0
    elif coherence < 0.95:
        problems.append("sub-bass phase")
    return {
        "file": path.name,
        "rate": rate,
        "channelsIn": channels,
        "dc": round(mean, 6),
        "peak": round(peak, 5),
        "rms": round(rms, 5),
        "crest": round(crest, 3),
        "intersamplePeak": round(intersample, 5),
        "subCoherence": round(coherence, 4),
        "punchWarning": punch_warning,
        "problems": problems,
        "passed": not problems,
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Audit a rendered drum WAV.")
    parser.add_argument("wav", type=Path)
    parser.add_argument("--json-out", type=Path)
    args = parser.parse_args(argv)
    report = audit(args.wav)
    text = json.dumps(report, indent=2)
    if args.json_out:
        args.json_out.write_text(text, encoding="utf-8")
    print(text)
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
