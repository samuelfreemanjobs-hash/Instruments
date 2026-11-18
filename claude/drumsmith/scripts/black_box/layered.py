"""Layered Black Box synthesis helpers — multi-body 808/snare/hat + velocity/stereo/bus.

Used when voice.layered is true. Complements phonk_* engines instead of replacing them.
"""

from __future__ import annotations

import math

import numpy as np

from scripts.memphis_oneshots.phonk_hat import generate_memphis_phonk_hat
from scripts.memphis_oneshots.phonk_kick import generate_memphis_phonk_kick
from scripts.memphis_oneshots.phonk_snare import generate_memphis_phonk_snare, _rbj_bandpass


def _norm(x: np.ndarray, ceiling_db: float = -0.3) -> np.ndarray:
    y = np.nan_to_num(x.astype(np.float64), nan=0.0, posinf=0.0, neginf=0.0)
    peak = float(np.max(np.abs(y))) or 1.0
    return ((y / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def apply_velocity(buf: np.ndarray, velocity: float, *, ceiling_db: float = -0.3) -> np.ndarray:
    """Soft/Med/Hard: preserve relative gain vs a hard reference peak.

    Soft must stay quieter than Hard after export — do not re-peak-normalize
    Soft to the same ceiling as Hard.
    """
    v = float(np.clip(velocity, 0.35, 1.0))
    x = np.nan_to_num(buf.astype(np.float64), nan=0.0, posinf=0.0, neginf=0.0)
    peak = float(np.max(np.abs(x))) or 1.0
    # Map velocity → linear gain relative to hard (v=1.0 → full ceiling)
    gain = 0.40 + 0.60 * v
    out = x * (gain / peak) * (10.0 ** (ceiling_db / 20.0))
    if v < 0.85:
        n = min(len(out), int(0.006 * 44100))
        out[:n] *= 0.65 + 0.35 * v
    return out.astype(np.float32)


def stereo_widen(mono: np.ndarray, width: float = 0.35) -> np.ndarray:
    """Return (2, N) float32 from mono with light Haas + mid/side width."""
    x = mono.astype(np.float64)
    delay = max(1, int(0.0007 * 44100))
    right = np.zeros_like(x)
    right[delay:] = x[:-delay]
    mid = 0.5 * (x + right)
    side = 0.5 * (x - right) * (1.0 + width)
    left = mid + side
    right = mid - side
    stacked = np.stack([left, right], axis=0)
    peak = float(np.max(np.abs(stacked))) or 1.0
    return (stacked / peak * (10.0 ** (-0.3 / 20.0))).astype(np.float32)


def bus_glue(buf: np.ndarray, *, drive_db: float = 2.5, lpf_hz: float = 14000.0) -> np.ndarray:
    """Light desk glue for mini-kits / construction kits."""
    x = buf.astype(np.float64)
    driven = np.tanh(x * (10.0 ** (drive_db / 20.0)))
    alpha = math.exp(-2.0 * math.pi * lpf_hz / 44100.0)
    y = np.zeros_like(driven)
    for i in range(1, len(driven)):
        y[i] = (1 - alpha) * driven[i] + alpha * y[i - 1]
    return _norm(y, ceiling_db=-0.2)


def layered_808(
    *,
    root_freq: float,
    duration: float,
    pitch_mod_depth: float,
    pitch_decay: float,
    amp_decay: float,
    click_amount: float,
    clip_drive_db: float,
    lofi_sr: float,
    bit_depth: int,
    tape_lpf_hz: float,
    seed: int,
    mid_bark: float = 0.22,
    sub_weight: float = 0.85,
) -> np.ndarray:
    """Sub sine + mid bark + beater click stacked before shared grit."""
    sub = generate_memphis_phonk_kick(
        root_freq=root_freq,
        duration=duration,
        pitch_mod_depth=pitch_mod_depth,
        pitch_decay=pitch_decay,
        amp_decay=amp_decay,
        click_amount=0.0,
        clip_drive_db=max(1.0, clip_drive_db - 1.5),
        lofi_sr=lofi_sr,
        bit_depth=bit_depth,
        tape_lpf_hz=tape_lpf_hz,
        seed=seed,
    )
    mid = generate_memphis_phonk_kick(
        root_freq=root_freq * 2.0,
        duration=min(duration, 0.55),
        pitch_mod_depth=max(8.0, pitch_mod_depth * 0.45),
        pitch_decay=max(0.008, pitch_decay * 0.7),
        amp_decay=min(0.18, amp_decay * 0.35),
        click_amount=0.0,
        clip_drive_db=clip_drive_db + 1.0,
        lofi_sr=min(lofi_sr, 22050),
        bit_depth=min(bit_depth, 11),
        tape_lpf_hz=min(tape_lpf_hz, 9000),
        seed=seed + 17,
    )
    click = generate_memphis_phonk_kick(
        root_freq=root_freq,
        duration=0.08,
        pitch_mod_depth=min(24.0, pitch_mod_depth * 0.4),
        pitch_decay=0.01,
        amp_decay=0.04,
        click_amount=max(click_amount, 0.35),
        clip_drive_db=clip_drive_db + 2.0,
        lofi_sr=lofi_sr,
        bit_depth=bit_depth,
        tape_lpf_hz=tape_lpf_hz,
        seed=seed + 33,
    )
    n = len(sub)
    mix = np.zeros(n, dtype=np.float64)
    mix += sub.astype(np.float64) * sub_weight
    mix[: len(mid)] += mid.astype(np.float64) * mid_bark
    mix[: len(click)] += click.astype(np.float64) * (0.35 + 0.4 * click_amount)
    return _norm(mix)


def layered_snare(
    *,
    membrane_root: float,
    pitch_mod_st: float,
    flam_delay_ms: float,
    clip_drive_db: float,
    seed: int,
    membrane_gain: float = 0.75,
    wires_gain: float = 0.55,
    body_gain: float = 0.35,
    noise_decay: float = 0.19,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
) -> np.ndarray:
    """Base snare + safe low body thump + bright wires shelf (no unstable triple BP)."""
    body = generate_memphis_phonk_snare(
        membrane_root=membrane_root,
        pitch_mod_st=pitch_mod_st,
        flam_delay_ms=flam_delay_ms,
        noise_decay=noise_decay,
        clip_drive_db=clip_drive_db,
        lofi_sr=lofi_sr,
        bit_depth=bit_depth,
        seed=seed,
    )
    # Short low thump (separate short render — safer than stacking three full snares)
    thump = generate_memphis_phonk_kick(
        root_freq=max(80.0, membrane_root * 0.45),
        duration=0.12,
        pitch_mod_depth=14.0,
        pitch_decay=0.012,
        amp_decay=0.05,
        click_amount=0.15,
        clip_drive_db=max(2.0, clip_drive_db - 1.0),
        lofi_sr=lofi_sr,
        bit_depth=bit_depth,
        tape_lpf_hz=6000.0,
        seed=seed + 19,
    )
    x = body.astype(np.float64) * (0.55 * membrane_gain + 0.45 * wires_gain)
    # Bright wires shelf via simple differentiator (stable)
    bright = np.diff(x, prepend=x[:1]) * (0.35 * wires_gain)
    mix = x + bright
    mix[: len(thump)] += thump.astype(np.float64) * body_gain
    return _norm(mix)


def layered_hat(
    *,
    open_hat: bool,
    center_hz: float,
    noise_decay: float,
    duration: float,
    clip_drive_db: float,
    bit_depth: int,
    lofi_sr: float,
    seed: int,
    metal: float = 1.0,
) -> np.ndarray:
    """Base EDGE hat + extra metallic partials."""
    base = generate_memphis_phonk_hat(
        duration=duration,
        center_hz=center_hz,
        noise_decay=noise_decay,
        seed=seed,
        clip_drive_db=clip_drive_db,
        bit_depth=bit_depth,
        lofi_sr=lofi_sr,
    )
    if metal <= 1.0 and not open_hat:
        return base
    sr = 44100.0
    nyquist = sr * 0.5
    # Keep RBJ bandpass centers strictly below Nyquist (Codex: BB00 Metal hats)
    max_hz = nyquist * 0.92
    f1 = min(center_hz * 1.55 * metal, max_hz)
    f2 = min(center_hz * 2.1 * metal, max_hz)
    if f2 <= 20.0:
        return base
    rng = np.random.default_rng(seed + 7)
    n = len(base)
    t = np.linspace(0.0, n / sr, n, endpoint=False)
    raw = rng.normal(0.0, 1.0, n)
    p1 = _rbj_bandpass(raw, sr, f1, 2.2)
    p2 = _rbj_bandpass(raw, sr, f2, 1.6)
    amp = np.exp(-t / max(0.008, noise_decay * 0.7))
    extras = (0.55 * p1 + 0.45 * p2) * amp * (0.25 + 0.2 * (metal - 1.0))
    return _norm(base.astype(np.float64) + extras)


def choke_pair(closed: np.ndarray, opened: np.ndarray, choke_ms: float = 40.0) -> np.ndarray:
    """Return open hat truncated as if choked by closed (for matched CH/OH sets)."""
    n_choke = int(choke_ms * 0.001 * 44100)
    out = opened.copy()
    if len(out) > n_choke:
        fade = np.linspace(1.0, 0.0, min(256, len(out) - n_choke))
        out[n_choke : n_choke + len(fade)] *= fade
        out[n_choke + len(fade) :] = 0.0
    return _norm(out)
