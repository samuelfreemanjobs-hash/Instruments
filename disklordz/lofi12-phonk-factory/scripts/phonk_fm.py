"""Two-operator FM for cowbells and metallic Memphis/808-style percussion."""

from __future__ import annotations

import math

from phonk_synth import SRC_RATE, noise


def fm_two_op(
    *,
    carrier_hz: float,
    mod_ratio: float,
    mod_index: float,
    duration: float,
    amp_decay: float,
    index_decay: float | None = None,
    seed: int = 0,
    grit: float = 0.0,
) -> list[float]:
    """Simple FM voice: index envelope decays faster than amp for bell-like timbre."""
    n = max(1, int(SRC_RATE * duration))
    idx_decay = index_decay if index_decay is not None else amp_decay * 0.35
    out: list[float] = []
    mod_phase = 0.0
    car_phase = 0.0
    for i in range(n):
        t = i / SRC_RATE
        amp_env = math.exp(-t / amp_decay)
        idx_env = math.exp(-t / idx_decay)
        mi = mod_index * idx_env
        mod_hz = carrier_hz * mod_ratio
        mod_phase += 2 * math.pi * mod_hz / SRC_RATE
        mod = math.sin(mod_phase) * mi
        car_phase += 2 * math.pi * carrier_hz / SRC_RATE
        sample = math.sin(car_phase + mod)
        if grit > 0:
            sample += noise(t, seed) * grit * 0.06 * amp_env
        out.append(sample * amp_env)
    return out


def fm_cowbell_808(base_hz: float, seed: int = 0, grit: float = 0.0) -> list[float]:
    """808-style dual-mode bell (two FM bursts summed)."""
    a = fm_two_op(
        carrier_hz=base_hz * 0.95,
        mod_ratio=1.42,
        mod_index=3.2,
        duration=0.24,
        amp_decay=0.085,
        index_decay=0.022,
        seed=seed,
        grit=grit,
    )
    b = fm_two_op(
        carrier_hz=base_hz * 1.38,
        mod_ratio=1.9,
        mod_index=2.4,
        duration=0.18,
        amp_decay=0.06,
        index_decay=0.018,
        seed=seed + 1,
        grit=grit * 0.5,
    )
    n = max(len(a), len(b))
    out: list[float] = []
    for i in range(n):
        out.append((a[i] if i < len(a) else 0.0) + (b[i] if i < len(b) else 0.0) * 0.65)
    return out


def fm_cowbell_memphis(base_hz: float, seed: int = 0, grit: float = 0.35) -> list[float]:
    """Darker, dirtier bell for phonk / Juicy J lane."""
    core = fm_two_op(
        carrier_hz=base_hz,
        mod_ratio=2.17,
        mod_index=4.0,
        duration=0.28,
        amp_decay=0.11,
        index_decay=0.028,
        seed=seed,
        grit=grit,
    )
    return core


def fm_rim_shot(seed: int = 0) -> list[float]:
    return fm_two_op(
        carrier_hz=420,
        mod_ratio=3.5,
        mod_index=5.5,
        duration=0.07,
        amp_decay=0.016,
        index_decay=0.008,
        seed=seed,
    )


def fm_clave(seed: int = 0) -> list[float]:
    return fm_two_op(
        carrier_hz=880,
        mod_ratio=2.8,
        mod_index=3.0,
        duration=0.04,
        amp_decay=0.012,
        index_decay=0.006,
        seed=seed,
    )


def normalize_peak(samples: list[float], peak: float = 0.89) -> list[float]:
    m = max(abs(s) for s in samples) or 1.0
    return [s * (peak / m) for s in samples]
