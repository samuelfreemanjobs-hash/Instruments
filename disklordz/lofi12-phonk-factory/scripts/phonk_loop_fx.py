"""Mix-bus FX for rendered phonk loops (filter, reverb, tape, drive)."""

from __future__ import annotations

import math
from dataclasses import asdict, dataclass

from phonk_synth import SRC_RATE, noise


@dataclass
class LoopFxParams:
    filter_cutoff: float = 0.65  # 0 dark – 1 bright (maps to Lofi CC 38 idea)
    reverb_send: float = 0.25  # 0–1 wet (CC 36)
    tape: float = 0.2
    drive: float = 0.15
    laid_back_ms: float = 0.0  # global micro delay (CC 31 vibe)


def fx_from_prompt(prompt: str) -> LoopFxParams:
    p = prompt.lower()
    fx = LoopFxParams()
    if "dark" in p or "screw" in p:
        fx.filter_cutoff = 0.42
        fx.tape = 0.35
    if "bright" in p or "clean" in p:
        fx.filter_cutoff = 0.82
    if "wet" in p or "space" in p or "reverb" in p:
        fx.reverb_send = 0.45
    if "dry" in p:
        fx.reverb_send = 0.08
    if "dirty" in p or "phonk" in p or "memphis" in p:
        fx.drive = 0.28
        fx.tape = max(fx.tape, 0.22)
    if "toomp" in p or "sp1200" in p or "sp-1200" in p:
        fx.filter_cutoff = min(fx.filter_cutoff, 0.5)
        fx.tape = max(fx.tape, 0.26)
        fx.drive = max(fx.drive, 0.3)
    if "drift" in p:
        fx.reverb_send = 0.32
    return fx


def _lowpass(samples: list[float], cutoff: float) -> list[float]:
    # cutoff 0..1 → coefficient
    a = min(0.995, max(0.05, 1.0 - cutoff * 0.92))
    out: list[float] = []
    state = 0.0
    for s in samples:
        state = a * state + (1 - a) * s
        out.append(state)
    return out


def _simple_reverb(samples: list[float], wet: float, seed: int = 0) -> list[float]:
    if wet <= 0.001:
        return samples
    delays = [int(SRC_RATE * t) for t in (0.031, 0.047, 0.061)]
    buf = list(samples)
    out = list(samples)
    for d in delays:
        for i in range(d, len(out)):
            out[i] += buf[i - d] * wet * 0.22
    for i in range(len(out)):
        out[i] += noise(i / SRC_RATE, seed) * wet * 0.015
    return out


def apply_loop_fx(samples: list[float], fx: LoopFxParams, *, seed: int = 0) -> list[float]:
    if not samples:
        return samples
    out = list(samples)
    if fx.laid_back_ms > 0:
        shift = int(SRC_RATE * (fx.laid_back_ms / 1000.0))
        if shift > 0:
            out = [0.0] * shift + out[:-shift]
    out = _lowpass(out, fx.filter_cutoff)
    drive = 1.0 + fx.drive * 3.5
    out = [math.tanh(s * drive) / math.tanh(drive) for s in out]
    if fx.tape > 0:
        for i in range(len(out)):
            t = i / SRC_RATE
            wobble = 1.0 + fx.tape * 0.02 * math.sin(t * 4.1 + seed)
            out[i] *= wobble
            out[i] += noise(t, seed + i) * fx.tape * 0.012
    wet = fx.reverb_send
    out = _simple_reverb(out, wet, seed)
    peak = max(abs(x) for x in out) or 1.0
    return [x * (0.89 / peak) for x in out]


def fx_to_lofi12_cc(fx: LoopFxParams) -> dict[str, int]:
    """Approximate CC values for manual Lofi-12 matching (0–127)."""
    return {
        "filterCutoff": int(max(0, min(127, fx.filter_cutoff * 127))),
        "reverbSend": int(max(0, min(127, fx.reverb_send * 127))),
        "laidBack": int(max(0, min(127, fx.laid_back_ms / 20.0 * 127))),
    }


def fx_dict(fx: LoopFxParams) -> dict:
    d = asdict(fx)
    d["lofi12Cc"] = fx_to_lofi12_cc(fx)
    return d
