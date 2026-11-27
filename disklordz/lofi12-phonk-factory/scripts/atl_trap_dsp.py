"""ATL trap DSP: SVF, saturation, sampler character, limiter (synthesized)."""

from __future__ import annotations

import math

from drum_dsp import SRC_RATE, bit_depth_reduce, one_pole_hp, one_pole_lp, zero_order_hold

PI2 = 2.0 * math.pi


def square(phase: float) -> float:
    return 1.0 if math.sin(phase) >= 0 else -1.0


def svf_lowpass(samples: list[float], fc: float, q: float = 0.7, sr: int = SRC_RATE) -> list[float]:
    if not samples:
        return []
    g = math.tan(PI2 * fc / (2.0 * sr))
    k = 1.0 / max(0.1, q)
    a1 = 1.0 / (1.0 + g * (g + k))
    a2 = g * a1
    a3 = g * a2
    ic1eq = ic2eq = 0.0
    out: list[float] = []
    for v in samples:
        v3 = v - ic2eq
        v1 = a1 * ic1eq + a2 * v3
        v2 = ic2eq + a2 * ic1eq + a3 * v3
        ic1eq = 2 * v1 - ic1eq
        ic2eq = 2 * v2 - ic2eq
        out.append(v2)
    return out


def svf_bandpass(samples: list[float], fc: float, q: float = 0.8, sr: int = SRC_RATE) -> list[float]:
    if not samples:
        return []
    g = math.tan(PI2 * fc / (2.0 * sr))
    k = 1.0 / max(0.1, q)
    a1 = 1.0 / (1.0 + g * (g + k))
    a2 = g * a1
    a3 = g * a2
    ic1eq = ic2eq = 0.0
    out: list[float] = []
    for v in samples:
        v3 = v - ic2eq
        v1 = a1 * ic1eq + a2 * v3
        v2 = ic2eq + a2 * ic1eq + a3 * v3
        ic1eq = 2 * v1 - ic1eq
        ic2eq = 2 * v2 - ic2eq
        out.append(v1)
    return out


def svf_highpass(samples: list[float], fc: float, q: float = 0.7, sr: int = SRC_RATE) -> list[float]:
    if not samples:
        return []
    g = math.tan(PI2 * fc / (2.0 * sr))
    k = 1.0 / max(0.1, q)
    a1 = 1.0 / (1.0 + g * (g + k))
    a2 = g * a1
    a3 = g * a2
    ic1eq = ic2eq = 0.0
    out: list[float] = []
    for v in samples:
        v3 = v - ic2eq
        v1 = a1 * ic1eq + a2 * v3
        v2 = ic2eq + a2 * ic1eq + a3 * v3
        ic1eq = 2 * v1 - ic1eq
        ic2eq = 2 * v2 - ic2eq
        hp = v - k * v1 - v2
        out.append(hp)
    return out


def biquad_bandpass(samples: list[float], fc: float, q: float = 1.1, sr: int = SRC_RATE) -> list[float]:
    w0 = PI2 * fc / sr
    alpha = math.sin(w0) / (2 * q)
    b0 = alpha
    b1 = 0.0
    b2 = -alpha
    a0 = 1 + alpha
    a1 = -2 * math.cos(w0)
    a2 = 1 - alpha
    x1 = x2 = y1 = y2 = 0.0
    out: list[float] = []
    for x in samples:
        y = (b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2) / a0
        x2, x1 = x1, x
        y2, y1 = y1, y
        out.append(y)
    return out


def tanh_asymmetric(samples: list[float], drive: float, bias: float = 0.08) -> list[float]:
    out = [math.tanh(s * drive + bias) - math.tanh(bias) for s in samples]
    peak = max(abs(s) for s in out) or 1.0
    return [s / peak for s in out]


def soft_knee_limit(samples: list[float], threshold: float = 0.92, knee: float = 0.08) -> list[float]:
    out: list[float] = []
    for s in samples:
        a = abs(s)
        sign = 1.0 if s >= 0 else -1.0
        if a <= threshold - knee:
            out.append(s)
        elif a < threshold + knee:
            t = (a - (threshold - knee)) / (2 * knee)
            compressed = (threshold - knee) + t * t * (2 * knee)
            out.append(sign * compressed)
        else:
            out.append(sign * threshold)
    return out


def sampler_2005_character(samples: list[float]) -> list[float]:
    out = zero_order_hold(samples, 2)
    out = bit_depth_reduce(out, 16)
    out = one_pole_lp(out, fc_hz=9500.0)
    out = tanh_asymmetric(out, drive=1.6, bias=0.06)
    return one_pole_hp(out, fc_hz=8.0)


def engine_c_polish(samples: list[float]) -> list[float]:
    out = tanh_asymmetric(samples, drive=1.15, bias=0.0)
    return soft_knee_limit(out, threshold=0.94, knee=0.1)


def integrate_exp_pitch(
    n: int, f_start: float, f_end: float, tau: float, sr: int = SRC_RATE
) -> tuple[list[float], list[float]]:
    """Return instantaneous freq and phase from integrated exponential pitch sweep."""
    freqs: list[float] = []
    phases: list[float] = []
    phase = 0.0
    for i in range(n):
        t = i / sr
        f = f_end + (f_start - f_end) * math.exp(-t / tau)
        freqs.append(f)
        phase += PI2 * f / sr
        phases.append(phase)
    return freqs, phases


def log_portamento_freq(n: int, f_from: float, f_to: float, sr: int = SRC_RATE, glide_ms: float = 40.0) -> list[float]:
    if n <= 0:
        return []
    tau = max(1e-3, glide_ms / 1000.0)
    out: list[float] = []
    lf = math.log(max(20.0, f_from))
    lt = math.log(max(20.0, f_to))
    for i in range(n):
        t = i / sr
        blend = 1.0 - math.exp(-t / tau)
        out.append(math.exp(lf + (lt - lf) * blend))
    return out
