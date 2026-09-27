"""Procedural phonk-oriented one-shots (44.1 kHz float lists)."""

from __future__ import annotations

import hashlib
import math
import re
from dataclasses import dataclass, replace
from typing import Callable

SRC_RATE = 44100


def sine(freq: float, t: float) -> float:
    return math.sin(2 * math.pi * freq * t)


def noise(t: float, seed: int) -> float:
    return math.sin(t * 9973.0 + seed * 0.17) * math.sin(t * 12347.0 + seed)


@dataclass
class PhonkParams:
    kick_pitch: float = 38.0
    kick_decay: float = 0.38
    snare_snap: float = 0.58
    snare_body: float = 0.22
    hat_bright: float = 0.5
    cowbell_tone: float = 680.0
    grit: float = 0.45
    seed: int = 0


def seed_from_prompt(prompt: str, variation: int = 0) -> int:
    h = hashlib.sha256(f"phonk::{prompt}::v={variation}".encode()).hexdigest()
    return int(h[:8], 16)


def resolve_phonk_params(prompt: str, variation: int = 0) -> PhonkParams:
    p = prompt.lower()
    seed = seed_from_prompt(prompt, variation)
    kick_pitch = 38.0
    kick_decay = 0.38
    snare_snap = 0.58
    snare_body = 0.22
    hat_bright = 0.5
    cowbell_tone = 680.0
    grit = 0.45

    if re.search(r"\b(808|sub|deep|low)\b", p):
        kick_pitch -= 5
        kick_decay *= 1.08
    if re.search(r"\b(fast|drift|160|150)\b", p):
        kick_decay *= 0.92
        hat_bright *= 1.08
    if re.search(r"\b(slow|screw|140|130)\b", p):
        kick_decay *= 1.15
        snare_snap *= 0.88
    if re.search(r"\b(dirty|grit|memphis|phonk|distort)\b", p):
        grit = min(0.85, grit + 0.25)
        snare_snap *= 1.12
    if re.search(r"\b(clean|minimal)\b", p):
        grit = max(0.15, grit - 0.2)
    if re.search(r"\b(cowbell|bell)\b", p):
        cowbell_tone *= 1.12

    jitter = lambda i: (((seed >> (i * 4)) & 0xF) / 0xF - 0.5) * 0.08
    return PhonkParams(
        kick_pitch=kick_pitch + jitter(0) * 6,
        kick_decay=max(0.15, kick_decay * (1 + jitter(1))),
        snare_snap=min(0.78, snare_snap * (1 + jitter(2))),
        snare_body=max(0.1, snare_body * (1 + jitter(3))),
        hat_bright=min(0.75, hat_bright * (1 + jitter(4))),
        cowbell_tone=cowbell_tone * (1 + jitter(5) * 0.1),
        grit=grit,
        seed=seed,
    )


def kick_808(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.55)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / params.kick_decay)
        f = params.kick_pitch * (1.0 + 5.0 * math.exp(-t * 28))
        out.append(sine(f, t) * env * 1.05)
    return out


def kick_distorted(params: PhonkParams) -> list[float]:
    base = kick_808(replace(params, kick_pitch=params.kick_pitch - 2))
    drive = 2.4 + params.grit * 2.0
    return [math.tanh(s * drive) / math.tanh(drive) for s in base]


def kick_sub(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.7)
    out: list[float] = []
    pitch = max(28.0, params.kick_pitch - 14)
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / (params.kick_decay * 1.6))
        out.append(sine(pitch, t) * env)
    return out


def snare_memphis(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.28)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.11)
        tone = sine(190, t) * params.snare_body
        nse = noise(t, params.seed) * params.snare_snap * math.exp(-t / 0.035)
        ring = sine(320, t) * 0.08 * math.exp(-t / 0.05)
        out.append((tone + nse + ring) * env)
    return out


def snare_rim(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.07)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.018)
        out.append((sine(420, t) + sine(880, t) * 0.55) * env * 0.9)
    return out


def clap_layer(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.2)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.1)
        burst = 0.0
        for off in (0.0, 0.011, 0.023, 0.036):
            if t >= off:
                burst += noise(t - off, params.seed + 11) * math.exp(-(t - off) / 0.022)
        out.append(burst * 0.42 * env)
    return out


def hat_closed(params: PhonkParams) -> list[float]:
    return _hat(params, decay=0.02, bright=params.hat_bright, seed_off=1)


def hat_open(params: PhonkParams) -> list[float]:
    return _hat(params, decay=0.055, bright=params.hat_bright * 1.15, seed_off=2)


def hat_pedal(params: PhonkParams) -> list[float]:
    return _hat(params, decay=0.035, bright=params.hat_bright * 0.85, seed_off=3)


def _hat(params: PhonkParams, decay: float, bright: float, seed_off: int) -> list[float]:
    n = int(SRC_RATE * 0.12)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        out.append(
            noise(t, params.seed + seed_off) * bright * env + sine(9000, t) * 0.04 * env
        )
    return out


def cowbell(freq: float, params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.22)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.09)
        partial = sine(freq, t) + sine(freq * 1.47, t) * 0.35 + sine(freq * 2.1, t) * 0.15
        out.append(partial * env * 0.55)
    return out


def tom(pitch: float, decay: float) -> list[float]:
    n = int(SRC_RATE * 0.35)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        out.append((sine(pitch, t) + sine(pitch * 0.5, t) * 0.25) * env * 0.7)
    return out


def perc_snap(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.05)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.012)
        out.append(noise(t, params.seed + 99) * env * 0.75)
    return out


def fx_impact(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.45)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.08)
        sweep = sine(80 + 400 * math.exp(-t * 6), t)
        out.append((sweep + noise(t, params.seed + 7) * 0.35) * env)
    return out


def fx_riser(params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 1.2)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = min(1.0, t / 0.15) * math.exp(-max(0, t - 0.9) / 0.25)
        f = 200 + 1800 * (t / 1.2)
        out.append((noise(t, params.seed + 3) * 0.4 + sine(f, t) * 0.12) * env)
    return out


RENDERERS: dict[str, Callable[[PhonkParams], list[float]]] = {
    "kick_808": lambda p: kick_808(p),
    "kick_dist": lambda p: kick_distorted(p),
    "kick_sub": lambda p: kick_sub(p),
    "snare_memphis": lambda p: snare_memphis(p),
    "snare_rim": lambda p: snare_rim(p),
    "clap": lambda p: clap_layer(p),
    "hat_closed": lambda p: hat_closed(p),
    "hat_open": lambda p: hat_open(p),
    "hat_pedal": lambda p: hat_pedal(p),
    "cowbell_low": lambda p: cowbell(p.cowbell_tone * 0.92, p),
    "cowbell_high": lambda p: cowbell(p.cowbell_tone * 1.35, p),
    "tom_low": lambda p: tom(95, 0.14),
    "tom_hi": lambda p: tom(160, 0.11),
    "perc_snap": lambda p: perc_snap(p),
    "fx_impact": lambda p: fx_impact(p),
    "fx_riser": lambda p: fx_riser(p),
}


def render_slot(key: str, params: PhonkParams) -> list[float]:
    fn = RENDERERS.get(key)
    if fn is None:
        raise KeyError(key)
    return fn(params)
