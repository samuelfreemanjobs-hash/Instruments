#!/usr/bin/env python3
"""Audio QA gates for factory outputs (used by QA subagent + CI)."""

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
    clipped_samples: int
    duration_s: float
    reasons: list[str]


def analyze_wav(path: Path) -> QaResult:
    samples, rate = read_mono_wav(path)
    reasons: list[str] = []
    if not samples:
        return QaResult(str(path), False, 0.0, 0.0, 0, 0.0, ["empty_file"])

    peak = max(abs(s) for s in samples)
    rms = math.sqrt(sum(s * s for s in samples) / len(samples))
    clipped = sum(1 for s in samples if abs(s) >= 0.999)
    duration = len(samples) / rate

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

    return QaResult(
        str(path),
        pass_qa=len(reasons) == 0,
        peak=peak,
        rms=rms,
        clipped_samples=clipped,
        duration_s=duration,
        reasons=reasons,
    )


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("wavs", nargs="+", type=Path)
    p.add_argument("--json-out", type=Path)
    p.add_argument("--fail-fast", action="store_true")
    args = p.parse_args()

    results = [analyze_wav(w) for w in args.wavs]
    ok = all(r.pass_qa for r in results)
    payload = {"pass": ok, "results": [asdict(r) for r in results]}
    text = json.dumps(payload, indent=2)
    print(text)
    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(text, encoding="utf-8")
    if args.fail_fast and not ok:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
