"""Synthetic Memphis/phonk vocal *textures* (formant chops — not artist voice clones)."""

from __future__ import annotations

import hashlib
import math
import re
from dataclasses import dataclass

from phonk_memphis_lanes import MemphisLane, resolve_memphis_lane

SRC_RATE = 44100

# Formant centers (Hz) for simple vowel-ish sources — generic, not speech synthesis of lyrics.
VOWELS: dict[str, tuple[float, float, float]] = {
    "uh": (650, 1100, 2400),
    "ah": (730, 1090, 2440),
    "oh": (570, 840, 2410),
    "ay": (660, 1720, 2600),
    "eh": (530, 1840, 2480),
}


@dataclass
class VocalParams:
    pitch_hz: float = 98.0
    screw: float = 0.82  # playback / pitch factor (lower = slower/darker)
    grit: float = 0.5
    telephone: float = 0.55  # band-pass mix
    seed: int = 0


def seed_from(prompt: str, variation: int) -> int:
    h = hashlib.sha256(f"vocal::{prompt}::v={variation}".encode()).hexdigest()
    return int(h[:10], 16)


def resolve_vocal_params(prompt: str, variation: int = 0, lane: MemphisLane | None = None) -> VocalParams:
    p = prompt.lower()
    seed = seed_from(prompt, variation)
    pitch = 102.0
    screw = 0.82
    grit = 0.48
    telephone = 0.55

    if re.search(r"\b(screw|slow|deep|dark)\b", p):
        screw *= 0.92
        pitch -= 8
    if re.search(r"\b(drift|phonk|dirty|grit)\b", p):
        grit = min(0.85, grit + 0.2)
        telephone = min(0.75, telephone + 0.1)
    if re.search(r"\b(clean|dry)\b", p):
        grit *= 0.6
        telephone *= 0.7

    if lane:
        if lane.lane_id in ("shawty_pimp", "toy_wright_iii", "kingpin_skinny_pimp"):
            screw *= 0.94
            pitch -= 6
        if lane.lane_id in ("dj_paul", "juicy_j", "blackout"):
            grit = min(0.88, grit + 0.08)

    j = lambda i: (((seed >> (i * 3)) & 0xFF) / 255.0 - 0.5) * 0.12
    return VocalParams(
        pitch_hz=max(72.0, pitch + j(0) * 20),
        screw=max(0.68, min(0.95, screw + j(1))),
        grit=max(0.15, min(0.9, grit + j(2))),
        telephone=max(0.25, min(0.85, telephone + j(3))),
        seed=seed,
    )


def _rng(seed: int, i: int) -> float:
    return ((seed >> (i % 24)) & 0xFF) / 255.0


def _source_glottal(phase: float, pitch: float, t: float) -> float:
    f0 = pitch
    return math.sin(2 * math.pi * f0 * t) * 0.6 + math.sin(2 * math.pi * f0 * 2 * t) * 0.2


def _formant_filter(sample: float, state: list[float], fc: float) -> float:
    # one-pole lowpass per formant peak approximation
    alpha = min(0.99, fc / (fc + SRC_RATE))
    state[0] = alpha * sample + (1 - alpha) * state[0]
    return state[0]


def render_vowel(
    vowel: str,
    duration: float,
    params: VocalParams,
    *,
    intensity: float = 1.0,
) -> list[float]:
    f1, f2, f3 = VOWELS.get(vowel, VOWELS["uh"])
    n = max(1, int(SRC_RATE * duration))
    out: list[float] = []
    s1, s2, s3 = [0.0], [0.0], [0.0]
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / (duration * 0.85)) * (1.0 - math.exp(-t / 0.012))
        src = _source_glottal(0, params.pitch_hz, t)
        src += (_rng(params.seed, i) - 0.5) * 0.08 * params.grit
        v = (
            _formant_filter(src, s1, f1) * 1.0
            + _formant_filter(src, s2, f2) * 0.55
            + _formant_filter(src, s3, f3) * 0.25
        )
        out.append(v * env * intensity)
    return phonkify(out, params)


def render_consonant_burst(duration: float, params: VocalParams, intensity: float = 0.7) -> list[float]:
    n = max(1, int(SRC_RATE * duration))
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.04)
        nse = (_rng(params.seed + 3, i) - 0.5) * 2.0
        out.append(nse * env * intensity * 0.35)
    return phonkify(out, params)


def phonkify(samples: list[float], params: VocalParams) -> list[float]:
    if not samples:
        return samples
    # screw = pitch down via slower read (resample)
    if params.screw < 0.999:
        stretched: list[float] = []
        pos = 0.0
        while int(pos) < len(samples):
            i0 = int(pos)
            frac = pos - i0
            i1 = min(i0 + 1, len(samples) - 1)
            stretched.append(samples[i0] * (1 - frac) + samples[i1] * frac)
            pos += params.screw
        samples = stretched

    # telephone band emphasis
    if params.telephone > 0.05:
        bp: list[float] = []
        state = 0.0
        for s in samples:
            state = 0.92 * state + 0.08 * s
            band = s - state
            bp.append(s * (1 - params.telephone) + band * params.telephone * 2.2)
        samples = bp

    drive = 1.0 + params.grit * 2.5
    samples = [math.tanh(x * drive) / math.tanh(drive) for x in samples]

    peak = max(abs(x) for x in samples) or 1.0
    return [x * (0.82 / peak) for x in samples]


def syllable_pattern_for_lane(lane: MemphisLane | None, seed: int, bars: int) -> list[tuple[str, float]]:
    """Sparse chant grid: (vowel_or_c, duration_sec). 'c' = noise burst."""
    base: list[tuple[str, float]] = [
        ("uh", 0.22),
        ("c", 0.05),
        ("ah", 0.18),
        ("uh", 0.2),
        ("oh", 0.24),
        ("c", 0.04),
    ]
    if lane and lane.lane_id in ("dj_paul", "juicy_j"):
        base = [("ay", 0.16), ("uh", 0.2), ("ah", 0.18), ("oh", 0.22), ("uh", 0.15)]
    if lane and lane.lane_id in ("shawty_pimp", "toy_wright_iii"):
        base = [("uh", 0.28), ("oh", 0.3), ("uh", 0.26)]
    if _rng(seed, 1) > 0.65:
        base = base[1:] + base[:1]
    reps = max(1, bars // 2)
    return base * reps


def render_vocal_loop(
    *,
    prompt: str,
    bpm: float,
    bars: int = 2,
    variation: int = 0,
    lane: MemphisLane | None = None,
) -> tuple[list[float], dict]:
    if lane is None:
        lane = resolve_memphis_lane(prompt, None)
    params = resolve_vocal_params(prompt, variation, lane)
    bar_sec = (60.0 / bpm) * 4
    target_len = int(SRC_RATE * bar_sec * bars)
    pattern = syllable_pattern_for_lane(lane, params.seed, bars)

    chunks: list[float] = []
    for sym, dur in pattern:
        if sym == "c":
            chunks.extend(render_consonant_burst(dur, params))
        else:
            chunks.extend(render_vowel(sym, dur, params))
        gap = int(SRC_RATE * 0.04)
        chunks.extend([0.0] * gap)

    if len(chunks) < target_len:
        while len(chunks) < target_len:
            chunks.extend(chunks[: max(1, target_len - len(chunks))])
    chunks = chunks[:target_len]

    meta = {
        "type": "synthetic_vocal_texture",
        "bpm": bpm,
        "bars": bars,
        "durationSec": round(len(chunks) / SRC_RATE, 4),
        "sampleRate": SRC_RATE,
        "memphisLane": lane.lane_id if lane else None,
        "pitchHz": round(params.pitch_hz, 2),
        "screw": round(params.screw, 3),
        "seed": params.seed,
        "disclaimer": "Formant synth — not a real performer; use your own acapellas for authentic samples.",
    }
    return chunks, meta


def render_vocal_chop_pack(
    prompt: str,
    variation: int = 0,
    lane: MemphisLane | None = None,
) -> dict[str, list[float]]:
    params = resolve_vocal_params(prompt, variation, lane)
    if lane is None:
        lane = resolve_memphis_lane(prompt, None)
    pack: dict[str, list[float]] = {}
    for v in ("uh", "ah", "oh", "ay"):
        pack[f"chop_{v}"] = render_vowel(v, 0.35, params, intensity=0.95)
    pack["chop_breath"] = render_consonant_burst(0.12, params, 0.5)
    pack["chop_hold"] = render_vowel("oh", 0.9, params, intensity=0.7)
    return pack
