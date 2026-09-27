"""Vintage drum machine sound engine for Memphis / phonk loop maker.

Inspired by TR-808/909, Boss DR-660, Alesis SR-16, Roland R-8 MKII, and
tape-screw aesthetics (e.g. lo-fi sample-pack style) — procedural originals only.
"""

from __future__ import annotations

import math
import re
from dataclasses import dataclass
from typing import Literal

from phonk_synth import PhonkParams, SRC_RATE, noise, sine

VoiceKey = Literal[
    "kick_808",
    "kick_dist",
    "kick_sub",
    "snare_memphis",
    "snare_rim",
    "clap",
    "hat_closed",
    "hat_open",
    "hat_pedal",
    "cowbell_low",
    "cowbell_high",
    "tom_low",
    "tom_hi",
    "perc_snap",
    "fx_impact",
    "fx_riser",
]

ENGINE_IDS = (
    "tr808",
    "tr909",
    "boss_dr660",
    "alesis_sr16",
    "roland_r8mk2",
    "dj_screw",
    "mr_tape",
    "classic",
)


@dataclass(frozen=True)
class MachineProfile:
    engine_id: str
    label: str
    kick_click: float
    kick_sweep: float
    kick_decay_mult: float
    snare_tone_hz: float
    snare_body_mult: float
    snare_noise_mult: float
    hat_decay_mult: float
    hat_dull: float  # 0 bright – 1 dull
    bits: int
    tape: float
    wow: float
    screw_rate: float  # playback stretch >1 = slower/darker
    drive: float
    hiss: float


PROFILES: dict[str, MachineProfile] = {
    "tr808": MachineProfile(
        "tr808",
        "Roland TR-808",
        0.08,
        5.2,
        1.15,
        185,
        0.22,
        1.0,
        1.0,
        0.15,
        16,
        0.05,
        0.0,
        1.0,
        1.1,
        0.02,
    ),
    "tr909": MachineProfile(
        "tr909",
        "Roland TR-909",
        0.35,
        3.8,
        0.82,
        210,
        0.18,
        1.15,
        0.75,
        0.05,
        16,
        0.03,
        0.0,
        1.0,
        1.35,
        0.015,
    ),
    "boss_dr660": MachineProfile(
        "boss_dr660",
        "Boss DR-660",
        0.12,
        4.2,
        0.95,
        175,
        0.28,
        0.92,
        0.9,
        0.35,
        12,
        0.08,
        0.01,
        1.0,
        1.25,
        0.03,
    ),
    "alesis_sr16": MachineProfile(
        "alesis_sr16",
        "Alesis SR-16",
        0.15,
        3.5,
        0.88,
        165,
        0.32,
        0.85,
        0.85,
        0.48,
        14,
        0.1,
        0.015,
        1.0,
        1.15,
        0.025,
    ),
    "roland_r8mk2": MachineProfile(
        "roland_r8mk2",
        "Roland R-8 MKII",
        0.2,
        4.0,
        0.92,
        195,
        0.38,
        0.95,
        0.95,
        0.22,
        16,
        0.06,
        0.008,
        1.0,
        1.2,
        0.02,
    ),
    "dj_screw": MachineProfile(
        "dj_screw",
        "DJ Screw / slowed tape",
        0.06,
        4.8,
        1.45,
        160,
        0.25,
        0.78,
        1.15,
        0.55,
        11,
        0.42,
        0.035,
        0.82,
        1.05,
        0.05,
    ),
    "mr_tape": MachineProfile(
        "mr_tape",
        "Tape-warped kit (Splice-style lo-fi)",
        0.18,
        4.4,
        1.05,
        180,
        0.3,
        0.88,
        0.95,
        0.4,
        12,
        0.38,
        0.028,
        0.95,
        1.28,
        0.055,
    ),
    "classic": MachineProfile(
        "classic",
        "Legacy phonk synth",
        0.1,
        5.0,
        1.0,
        190,
        0.22,
        1.0,
        1.0,
        0.2,
        16,
        0.0,
        0.0,
        1.0,
        1.0,
        0.0,
    ),
}


def resolve_engine_id(prompt: str, override: str | None = None) -> str:
    if override:
        key = override.strip().lower().replace("-", "_")
        if key in PROFILES:
            return key
        for eid in ENGINE_IDS:
            if key in eid:
                return eid
    p = prompt.lower()
    rules: list[tuple[str, str]] = [
        (r"\b(mr\s*tape|splice|tape\s*pack)\b", "mr_tape"),
        (r"\b(screw|dj\s*screw|slowed)\b", "dj_screw"),
        (r"\b(909|tr\s*909)\b", "tr909"),
        (r"\b(808|tr\s*808)\b", "tr808"),
        (r"\b(dr\s*660|dr660|boss)\b", "boss_dr660"),
        (r"\b(sr\s*16|sr16|alesis)\b", "alesis_sr16"),
        (r"\b(r\s*8|r8mk2|r8\s*mk)\b", "roland_r8mk2"),
        (r"\b(memphis|phonk|90s)\b", "mr_tape"),
    ]
    for pat, eid in rules:
        if re.search(pat, p):
            return eid
    return "mr_tape"


def _bitcrush(samples: list[float], bits: int) -> list[float]:
    if bits >= 16:
        return samples
    levels = 2**bits
    return [round(s * levels) / levels for s in samples]


def _lowpass(samples: list[float], coeff: float) -> list[float]:
    if coeff <= 0:
        return samples
    out: list[float] = []
    state = 0.0
    a = min(0.99, coeff)
    for s in samples:
        state = a * state + (1 - a) * s
        out.append(state)
    return out


def _tape_chain(samples: list[float], prof: MachineProfile, seed: int) -> list[float]:
    if prof.tape <= 0 and prof.hiss <= 0 and prof.wow <= 0:
        return samples
    out: list[float] = []
    phase = 0.0
    for i, s in enumerate(samples):
        t = i / SRC_RATE
        wow = 1.0 + prof.wow * math.sin(t * 3.7 + seed * 0.001)
        phase += wow / SRC_RATE / prof.screw_rate
        idx = int(phase * SRC_RATE)
        if idx >= len(samples):
            break
        v = samples[min(idx, len(samples) - 1)]
        v += (noise(t, seed + i) * prof.hiss * 0.08)
        drive = prof.drive * (1 + prof.tape * 0.5)
        v = math.tanh(v * drive) / math.tanh(drive)
        out.append(v)
    return out if out else samples


def _finish(samples: list[float], prof: MachineProfile, seed: int) -> list[float]:
    if prof.hat_dull > 0:
        samples = _lowpass(samples, prof.hat_dull * 0.85)
    samples = _bitcrush(samples, prof.bits)
    samples = _tape_chain(samples, prof, seed)
    peak = max(abs(x) for x in samples) or 1.0
    return [x * (0.89 / peak) for x in samples]


def _kick(prof: MachineProfile, params: PhonkParams) -> list[float]:
    decay = params.kick_decay * prof.kick_decay_mult
    n = int(SRC_RATE * 0.55 * prof.screw_rate)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        f = params.kick_pitch * (1.0 + prof.kick_sweep * math.exp(-t * 30))
        click = prof.kick_click * math.exp(-t / 0.004) * noise(t, params.seed)
        out.append((sine(f, t) * env * 1.05 + click) * env)
    return _finish(out, prof, params.seed)


def _kick_dist(prof: MachineProfile, params: PhonkParams) -> list[float]:
    base = _kick(prof, params)
    drive = 2.2 + params.grit * 2.0 + prof.drive * 0.3
    return _finish([math.tanh(s * drive) / math.tanh(drive) for s in base], prof, params.seed + 1)


def _kick_sub(prof: MachineProfile, params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.65 * prof.screw_rate)
    out: list[float] = []
    pitch = max(28.0, params.kick_pitch - 14)
    decay = params.kick_decay * prof.kick_decay_mult * 1.5
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        out.append(sine(pitch, t) * env)
    return _finish(out, prof, params.seed + 2)


def _snare(prof: MachineProfile, params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.28)
    out: list[float] = []
    tone_hz = prof.snare_tone_hz
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.11)
        tone = sine(tone_hz, t) * params.snare_body * prof.snare_body_mult
        nse = (
            noise(t, params.seed)
            * params.snare_snap
            * prof.snare_noise_mult
            * math.exp(-t / 0.035)
        )
        out.append((tone + nse) * env)
    return _finish(out, prof, params.seed + 3)


def _rim(prof: MachineProfile, params: PhonkParams) -> list[float]:
    n = int(SRC_RATE * 0.07)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.018)
        out.append((sine(420, t) + sine(880, t) * 0.5) * env * 0.9)
    return _finish(out, prof, params.seed + 4)


def _clap(prof: MachineProfile, params: PhonkParams) -> list[float]:
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
    return _finish(out, prof, params.seed + 5)


def _hat(prof: MachineProfile, params: PhonkParams, decay: float, bright: float, off: int) -> list[float]:
    decay *= prof.hat_decay_mult
    n = int(SRC_RATE * 0.12)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        out.append(
            noise(t, params.seed + off) * bright * env + sine(9000, t) * 0.04 * env * (1 - prof.hat_dull)
        )
    return _finish(out, prof, params.seed + 6 + off)


def _cowbell(prof: MachineProfile, params: PhonkParams, freq: float) -> list[float]:
    n = int(SRC_RATE * 0.22)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / 0.09)
        partial = sine(freq, t) + sine(freq * 1.47, t) * 0.35 + sine(freq * 2.1, t) * 0.15
        out.append(partial * env * 0.55)
    return _finish(out, prof, params.seed + 8)


def _tom(prof: MachineProfile, params: PhonkParams, pitch: float, decay: float) -> list[float]:
    n = int(SRC_RATE * 0.35)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = math.exp(-t / decay)
        out.append((sine(pitch, t) + sine(pitch * 0.5, t) * 0.25) * env * 0.7)
    return _finish(out, prof, params.seed + 9)


def render_machine_voice(engine_id: str, key: VoiceKey, params: PhonkParams) -> list[float]:
    prof = PROFILES.get(engine_id, PROFILES["mr_tape"])
    if engine_id == "classic":
        from phonk_synth import RENDERERS

        fn = RENDERERS.get(key)
        if fn is None:
            raise KeyError(key)
        return fn(params)

    if key == "kick_808":
        return _kick(prof, params)
    if key == "kick_dist":
        return _kick_dist(prof, params)
    if key == "kick_sub":
        return _kick_sub(prof, params)
    if key == "snare_memphis":
        return _snare(prof, params)
    if key == "snare_rim":
        return _rim(prof, params)
    if key == "clap":
        return _clap(prof, params)
    if key == "hat_closed":
        return _hat(prof, params, 0.02, params.hat_bright, 1)
    if key == "hat_open":
        return _hat(prof, params, 0.055, params.hat_bright * 1.15, 2)
    if key == "hat_pedal":
        return _hat(prof, params, 0.035, params.hat_bright * 0.85, 3)
    if key == "cowbell_low":
        return _cowbell(prof, params, params.cowbell_tone * 0.92)
    if key == "cowbell_high":
        return _cowbell(prof, params, params.cowbell_tone * 1.35)
    if key == "tom_low":
        return _tom(prof, params, 95, 0.14)
    if key == "tom_hi":
        return _tom(prof, params, 160, 0.11)
    if key == "perc_snap":
        n = int(SRC_RATE * 0.05)
        out = [noise(i / SRC_RATE, params.seed + 99) * math.exp(-(i / SRC_RATE) / 0.012) * 0.75 for i in range(n)]
        return _finish(out, prof, params.seed + 10)
    if key == "fx_impact":
        from phonk_synth import fx_impact

        return _finish(fx_impact(params), prof, params.seed + 11)
    if key == "fx_riser":
        from phonk_synth import fx_riser

        return _finish(fx_riser(params), prof, params.seed + 12)
    raise KeyError(key)


def list_engines() -> list[tuple[str, str]]:
    return [(p.engine_id, p.label) for p in PROFILES.values() if p.engine_id != "classic"]
