"""1990s Memphis / phonk drum programming — groove templates and density rules."""

from __future__ import annotations

import hashlib
from dataclasses import dataclass, field
from typing import Literal

Instrument = Literal["kick", "kick_dist", "snare", "clap", "hat", "hat_open", "cowbell", "rim"]

STEPS_PER_BAR = 16
BEATS_PER_BAR = 4


@dataclass
class Hit:
    step: int  # 0..15 within bar
    instrument: Instrument
    velocity: float  # 0..1
    micro_delay: float = 0.0  # seconds late (+ = laid back)


@dataclass
class BarPattern:
    hits: list[Hit] = field(default_factory=list)


# Classic Memphis / 808 grids (16th-note steps). No trap 32nd rolls or double-time hats.
KICK_TEMPLATES: tuple[tuple[int, ...], ...] = (
    (0, 6, 8),  # on the 1, syncopated &, 3
    (0, 10),  # half-time lean
    (0, 3, 6, 11),  # DJ Paul bounce
    (0, 8, 14),  # sparse hard
    (0, 6, 8, 14),  # four-on-floor variant with tail kick
)

SNARE_BACKBEAT = (4, 12)
COWBELL_CANDIDATES = (2, 7, 9, 13, 15)


def seed_int(*parts: str) -> int:
    h = hashlib.sha256("::".join(parts).encode()).hexdigest()
    return int(h[:12], 16)


def effective_groove_bpm(bpm: float) -> float:
    """Cap perceived density — fast BPM uses halftime programming, not faster hats."""
    if bpm <= 120:
        return bpm
    if bpm <= 160:
        return bpm * 0.5
    return bpm * 0.55


def max_hats_per_bar(bpm: float) -> int:
    eg = effective_groove_bpm(bpm)
    if eg <= 75:
        return 10
    if eg <= 95:
        return 8
    if eg <= 110:
        return 6
    return 4  # drift BPM metadata high; keep hats sparse


def swing_ratio(bpm: float, seed: int) -> float:
    """Memphis shuffle: ~57–62% on 8ths (not straight trap)."""
    base = 0.58 if bpm < 100 else 0.55
    jitter = ((seed >> 4) & 0xF) / 0xF * 0.06
    return min(0.64, max(0.52, base + jitter))


def _rng(seed: int, i: int) -> float:
    return ((seed >> (i % 20)) & 0xFF) / 255.0


def build_bar(
    bar_index: int,
    *,
    bpm: float,
    seed: int,
    variation: int,
) -> BarPattern:
    s = seed_int(str(seed), str(variation), str(bar_index))
    kick_idx = (s + bar_index) % len(KICK_TEMPLATES)
    kick_steps = KICK_TEMPLATES[kick_idx]
    hits: list[Hit] = []

    for st in kick_steps:
        vel = 0.88 + _rng(s, st) * 0.1
        hits.append(Hit(st, "kick", vel))
        if _rng(s, st + 3) > 0.72:
            hits.append(Hit(st, "kick_dist", vel * 0.35, micro_delay=0.004))

    for st in SNARE_BACKBEAT:
        if bar_index == 1 and st == 12 and _rng(s, 99) > 0.5:
            continue  # occasional bar-2 fill drop
        hits.append(Hit(st, "snare", 0.78 + _rng(s, st + 1) * 0.15))
        if _rng(s, st + 7) > 0.35:
            hits.append(Hit(st, "clap", 0.42, micro_delay=0.006))
        if bar_index == 1 and st == 12 and _rng(s, 50) > 0.6:
            hits.append(Hit(14, "rim", 0.55))

    hat_cap = max_hats_per_bar(bpm)
    hat_steps: list[int] = []
    for st in range(0, STEPS_PER_BAR, 2):  # 8th-note grid base
        if len(hat_steps) >= hat_cap:
            break
        if st in kick_steps and _rng(s, st + 20) < 0.55:
            continue  # leave space under kicks
        hat_steps.append(st)
    if effective_groove_bpm(bpm) <= 90 and len(hat_steps) < hat_cap:
        extra = 1 if _rng(s, 88) > 0.5 else 3
        if extra not in hat_steps:
            hat_steps.append(extra)

    sw = swing_ratio(bpm, s)
    step_sec = (60.0 / bpm) / 4  # 16th duration
    for i, st in enumerate(sorted(hat_steps)):
        vel = 0.28 + _rng(s, 30 + i) * 0.22
        delay = step_sec * sw * 0.45 if st % 2 == 1 else 0.0
        hits.append(Hit(st, "hat", vel, micro_delay=delay))
        if st == 6 and _rng(s, 44) > 0.7:
            hits.append(Hit(st, "hat_open", 0.32, micro_delay=delay))

    bell_count = 1 if effective_groove_bpm(bpm) > 100 else 2
    bells = sorted(COWBELL_CANDIDATES, key=lambda c: _rng(s, c))[:bell_count]
    for st in bells:
        if st not in kick_steps:
            hits.append(Hit(st, "cowbell", 0.4 + _rng(s, st + 5) * 0.25))

    return BarPattern(hits=hits)


def build_loop_pattern(
    *,
    bpm: float,
    bars: int,
    seed: int,
    variation: int,
) -> list[BarPattern]:
    if bars not in (1, 2, 4):
        bars = 2 if bpm >= 100 else 4
    out: list[BarPattern] = []
    for b in range(bars):
        out.append(build_bar(b, bpm=bpm, seed=seed, variation=variation))
    return out


def validate_bpm(bpm: float) -> float:
    return min(190.0, max(60.0, bpm))
