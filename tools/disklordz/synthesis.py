"""Time-domain drum synthesis (aligned with docs/DISKLORDZ_BLUEPRINT.md)."""

from __future__ import annotations

import numpy as np

DEFAULT_SR = 44100


def _t(seconds: float, sr: int) -> np.ndarray:
    n = max(1, int(sr * seconds))
    return np.linspace(0.0, seconds, n, endpoint=False, dtype=np.float64)


def apply_overdrive(audio: np.ndarray, amount: float) -> np.ndarray:
    if amount <= 0.0:
        return audio
    return np.clip(audio * (1.0 + amount * 10.0), -0.9, 0.9)


def sp1200_chain(audio: np.ndarray, grit: float = 0.5, sr: int = DEFAULT_SR) -> np.ndarray:
    """12-bit quantize, optional SP-style decimation, clip."""
    bits = 2**12
    x = np.round(audio * (bits / 2)) / (bits / 2)
    target_sr = 26040.0
    if sr > target_sr:
        factor = int(round(sr / target_sr))
        if factor > 1:
            x = x[::factor]
            x = np.repeat(x, factor)[: len(audio)]
    return apply_overdrive(x, grit)


def kick_808_phonk(
    sr: int = DEFAULT_SR,
    f_start: float = 45.0,
    f_base: float = 42.0,
    pitch_decay: float = 5.0,
    amp_decay: float = 2.0,
    duration: float = 1.5,
    sp1200: bool = False,
    grit: float = 0.5,
) -> np.ndarray:
    t = _t(duration, sr)
    f_env = f_start * np.exp(-pitch_decay * t) + f_base
    phase = 2.0 * np.pi * np.cumsum(f_env) / sr
    audio = np.sin(phase) * np.exp(-amp_decay * t)
    if sp1200:
        audio = sp1200_chain(audio, grit=grit, sr=sr)
    else:
        audio = apply_overdrive(audio, grit)
    peak = np.max(np.abs(audio)) or 1.0
    return (audio / peak).astype(np.float64)


def kick_trap(
    sr: int = DEFAULT_SR,
    f_base: float = 40.0,
    decay: float = 1.2,
    transient_snap: float = 0.9,
) -> np.ndarray:
    t = _t(decay, sr)
    p_env = (f_base * 4.0) * np.exp(-30.0 * t) + f_base
    phase = 2.0 * np.pi * np.cumsum(p_env) / sr
    audio = np.sin(phase) * np.exp(-1.5 * t / decay)
    audio = np.clip(audio * (1.0 + transient_snap), -0.9, 0.9)
    peak = np.max(np.abs(audio)) or 1.0
    return (audio / peak).astype(np.float64)


def obsidian_kick(
    sr: int = DEFAULT_SR,
    f_base: float = 55.0,
    f_floor: float = 38.0,
    alpha: float = 20.0,
    beta: float = 11.0,
    gamma: float = 50.0,
    noise_amp: float = 0.25,
    drive: float = 3.2,
    duration: float = 0.8,
) -> np.ndarray:
    """Sub sweep + noise transient + tanh saturation (enterprise SonicFactory)."""
    t = _t(duration, sr)
    f_t = f_base * np.exp(-alpha * t) + f_floor
    phase = 2.0 * np.pi * np.cumsum(f_t) / sr
    x_sub = np.exp(-beta * t) * np.sin(phase)
    noise = np.random.uniform(-1.0, 1.0, len(t))
    x_noise = noise * np.exp(-gamma * t) * noise_amp
    y = np.tanh(drive * (x_sub + x_noise))
    peak = np.max(np.abs(y)) or 1.0
    return (y / peak).astype(np.float64)


def cowbell_memphis(
    sr: int = DEFAULT_SR,
    freq: float = 587.33,
    decay: float = 0.8,
    grit: float = 0.5,
) -> np.ndarray:
    t = _t(decay, sr)
    w1 = np.sign(np.sin(2.0 * np.pi * freq * t))
    w2 = np.sign(np.sin(2.0 * np.pi * freq * 1.481 * t))
    audio = (w1 + w2) * 0.5 * np.exp(-6.0 * t / decay)
    audio = apply_overdrive(audio, grit)
    peak = np.max(np.abs(audio)) or 1.0
    return (audio / peak).astype(np.float64)


def hat_crashed(
    sr: int = DEFAULT_SR,
    brightness: float = 0.9,
    decay: float = 0.15,
    seed: int | None = None,
) -> np.ndarray:
    rng = np.random.default_rng(seed)
    t = _t(decay, sr)
    noise = rng.uniform(-1.0, 1.0, len(t))
    hp = np.diff(noise, prepend=0.0)
    audio = hp * np.exp(-25.0 * t / decay) * brightness
    peak = np.max(np.abs(audio)) or 1.0
    return (audio / peak).astype(np.float64)


def snare_trap(sr: int = DEFAULT_SR, seed: int | None = None) -> np.ndarray:
    rng = np.random.default_rng(seed)
    decay = 0.2
    t = _t(decay, sr)
    noise = rng.uniform(-1.0, 1.0, len(t))
    body = np.sin(2.0 * np.pi * 180.0 * t) * np.exp(-20.0 * t)
    audio = noise * np.exp(-40.0 * t) + body * 0.5
    peak = np.max(np.abs(audio)) or 1.0
    return (audio / peak).astype(np.float64)


def apply_memphis_crunch(audio: np.ndarray, seed: int | None = None) -> np.ndarray:
    rng = np.random.default_rng(seed)
    x = np.clip(audio * 1.5, -0.7, 0.7)
    hiss = rng.normal(0.0, 0.002, len(x))
    return x + hiss


def apply_tape_saturation(audio: np.ndarray) -> np.ndarray:
    return np.tanh(audio * 2.0)


def to_int16(audio: np.ndarray, headroom: float = 0.95) -> np.ndarray:
    return (np.clip(audio, -1.0, 1.0) * 32767.0 * headroom).astype(np.int16)
