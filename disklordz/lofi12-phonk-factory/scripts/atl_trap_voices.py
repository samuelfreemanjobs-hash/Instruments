"""Early-2000s Atlanta trap — synthesized voices (Sam DSP reference)."""

from __future__ import annotations

import math

from atl_trap_dsp import (
    biquad_bandpass,
    integrate_exp_pitch,
    log_portamento_freq,
    square,
    svf_bandpass,
    svf_highpass,
    svf_lowpass,
    tanh_asymmetric,
)
from phonk_synth import PhonkParams, SRC_RATE, noise, sine


def _hp_click(n: int, seed: int, sr: int = SRC_RATE) -> list[float]:
    click = [0.0] * n
    for i in range(min(n, int(sr * 0.006))):
        t = i / sr
        click[i] = noise(t, seed + 2) * math.exp(-t / 0.0012)
    return svf_highpass(click, 180.0, q=0.8, sr=sr)


def kick_atl_round(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.62)
    f_start = params.kick_pitch * 2.8
    f_end = max(32.0, params.kick_pitch)
    _, phases = integrate_exp_pitch(n, f_start, f_end, tau=0.045, sr=sr)
    click = _hp_click(n, params.seed, sr)
    out: list[float] = []
    for i in range(n):
        t = i / sr
        env = math.exp(-t / max(0.2, params.kick_decay))
        body = math.sin(phases[i])
        shaped = math.tanh(body * 1.25) * env
        out.append(shaped + click[i] * 0.35)
    return out


def kick_atl_hard(params: PhonkParams) -> list[float]:
    raw = kick_atl_round(params)
    return tanh_asymmetric(raw, drive=2.2 + params.grit, bias=0.12)


def sub_atl_long(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 1.08)
    f_rest = max(26.0, params.kick_pitch - 14)
    f_hit = params.kick_pitch
    freqs = log_portamento_freq(n, f_hit, f_rest, sr=sr, glide_ms=55.0)
    phase = 0.0
    out: list[float] = []
    for i in range(n):
        t = i / sr
        phase += 2 * math.pi * freqs[i] / sr
        env = math.exp(-t / (params.kick_decay * 2.4))
        body = math.tanh(math.sin(phase) * (1.3 + params.grit)) * env
        out.append(body)
    return out


def clap_atl_grit(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.22)
    bursts = [0.0, 0.010, 0.020, 0.030]
    raw = [0.0] * n
    for bi, off in enumerate(bursts):
        for i in range(n):
            t = i / sr
            if t < off:
                continue
            dt = t - off
            env = (1 - math.exp(-dt / 0.0015)) * math.exp(-dt / 0.022)
            raw[i] += noise(dt, params.seed + bi * 3) * env
    tail = [noise(i / sr, params.seed + 40) * math.exp(-(i / sr) / 0.09) * 0.15 for i in range(n)]
    mixed = [raw[i] + tail[i] for i in range(n)]
    fc = 1200.0 + params.snare_snap * 700.0
    return [s * 0.7 for s in biquad_bandpass(mixed, fc, q=1.15, sr=sr)]


def _hat_cluster(params: PhonkParams, *, decay: float, seed_off: int) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.14)
    ratios = (1.0, 1.37, 1.91, 2.43, 3.07, 3.88)
    base = 310.0 + (params.seed % 37)
    raw = [0.0] * n
    for i in range(n):
        t = i / sr
        env = math.exp(-t / decay)
        s = 0.0
        for j, r in enumerate(ratios):
            phase = 2 * math.pi * base * r * t
            s += square(phase) * (0.22 / (j + 1))
        nse = noise(t, params.seed + seed_off) * 0.08
        raw[i] = (s + nse) * env
    a = svf_bandpass(raw, 5200.0, q=0.9, sr=sr)
    b = svf_bandpass(raw, 9800.0, q=0.75, sr=sr)
    mix = [a[i] * 0.55 + b[i] * 0.45 for i in range(n)]
    return svf_highpass(mix, 7000.0, q=0.65, sr=sr)


def hat_atl_closed(params: PhonkParams) -> list[float]:
    return _hat_cluster(params, decay=0.018, seed_off=5)


def hat_atl_open(params: PhonkParams) -> list[float]:
    return _hat_cluster(params, decay=0.095, seed_off=6)


def snare_atl_bridged(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.32)
    tone_raw: list[float] = []
    noise_raw: list[float] = []
    f0 = 185.0
    for i in range(n):
        t = i / sr
        f = f0 * (1.0 + 0.8 * math.exp(-t * 20))
        env_t = math.exp(-t / 0.12)
        bridge = math.sin(2 * math.pi * f * t) + math.sin(2 * math.pi * f * 1.97 * t) * 0.4
        tone_raw.append(bridge * env_t * params.snare_body)
        env_n = math.exp(-t / 0.065)
        noise_raw.append(noise(t, params.seed + 9) * params.snare_snap * env_n)
    tone = svf_bandpass(tone_raw, 420.0, q=0.85, sr=sr)
    nse = svf_bandpass(noise_raw, 2400.0, q=0.9, sr=sr)
    return [tone[i] * 0.6 + nse[i] * 0.75 for i in range(n)]


def tom_tuned(params: PhonkParams, *, pitch_hz: float) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.45)
    out: list[float] = []
    phase = 0.0
    for i in range(n):
        t = i / sr
        f = pitch_hz * (1.0 + 1.2 * math.exp(-t * 14))
        phase += 2 * math.pi * f / sr
        env = math.exp(-t / 0.17)
        out.append(math.sin(phase) * env * 0.8)
    return svf_lowpass(out, 4200.0, q=0.7, sr=sr)


def tom_descending(params: PhonkParams) -> list[float]:
    return tom_tuned(params, pitch_hz=175.0)


def crash_cymbal(params: PhonkParams) -> list[float]:
    raw = _hat_cluster(params, decay=1.25, seed_off=19)
    n = len(raw)
    out = [raw[i] * (1 + 0.35 * math.sin(i * 0.01)) for i in range(n)]
    return [s * 0.55 for s in out]


def ride_cymbal(params: PhonkParams) -> list[float]:
    return _hat_cluster(params, decay=0.75, seed_off=21)


def fm_rim(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.08)
    out: list[float] = []
    fc = 320.0
    for i in range(n):
        t = i / sr
        env = math.exp(-t / 0.012)
        mod = math.sin(2 * math.pi * fc * 2.3 * t)
        car = math.sin(2 * math.pi * fc * t + mod * 2.8)
        out.append(car * env * 0.75)
    return out


def fm_woodblock(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.06)
    out: list[float] = []
    fc = 660.0 + (params.seed % 5) * 12
    for i in range(n):
        t = i / sr
        env = math.exp(-t / 0.018)
        mod = math.sin(2 * math.pi * fc * 1.7 * t)
        car = math.sin(2 * math.pi * fc * t + mod * 1.6)
        out.append(car * env * 0.6)
    return out


def fm_cowbell_atl(params: PhonkParams) -> list[float]:
    sr = SRC_RATE
    n = int(sr * 0.2)
    f = params.cowbell_tone * 0.95
    out: list[float] = []
    for i in range(n):
        t = i / sr
        env = math.exp(-t / 0.08)
        mod = math.sin(2 * math.pi * f * 1.41 * t)
        car = math.sin(2 * math.pi * f * t + mod * 2.2)
        out.append(car * env * 0.5)
    return out
