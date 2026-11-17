"""dsp_math.py — deterministic DSP helper math for DRUMSMITH.

stdlib only. Every number DRUMSMITH reports about pitch, level, loop length,
glide or fades comes from these functions, never from model inference.
Each function states its formula. Tests live in scripts/self_test.py with
hand-derived expected values.
"""
import math
from decimal import Decimal, getcontext, ROUND_HALF_EVEN

getcontext().prec = 28
getcontext().rounding = ROUND_HALF_EVEN


def midi_to_hz(note: float) -> float:
    """f = 440 * 2^((note - 69) / 12)"""
    return 440.0 * 2.0 ** ((note - 69.0) / 12.0)


def cents_between(f_measured: float, f_target: float) -> float:
    """cents = 1200 * log2(f_measured / f_target). Halts on non-positive input."""
    if f_measured <= 0 or f_target <= 0:
        raise ValueError("cents_between: frequencies must be > 0")
    return 1200.0 * math.log2(f_measured / f_target)


def dbfs(linear: float) -> float:
    """dBFS = 20 * log10(|linear|). Halts on zero (silence has no dBFS)."""
    if linear == 0:
        raise ValueError("dbfs: amplitude must be non-zero")
    return 20.0 * math.log10(abs(linear))


def db_to_linear(db: float) -> float:
    """linear = 10^(db / 20)"""
    return 10.0 ** (db / 20.0)


def loop_samples_exact(bars: int, bpm: float, sr: int, beats_per_bar: int = 4) -> Decimal:
    """exact samples = bars * beats_per_bar * 60 * sr / bpm. Halts on bars/bpm/sr <= 0."""
    if bars <= 0 or bpm <= 0 or sr <= 0 or beats_per_bar <= 0:
        raise ValueError("loop_samples: bars, bpm, sr, beats_per_bar must be > 0")
    return (Decimal(bars) * Decimal(beats_per_bar) * Decimal(60) * Decimal(sr)) / Decimal(str(bpm))


def loop_samples(bars: int, bpm: float, sr: int, beats_per_bar: int = 4) -> int:
    """Rendered loop length: exact value rounded half-even to a whole sample."""
    return int(loop_samples_exact(bars, bpm, sr, beats_per_bar).quantize(Decimal("1")))


def sixteenth_ms(bpm: float) -> float:
    """one sixteenth note in ms = 15000 / bpm (4/4)"""
    if bpm <= 0:
        raise ValueError("sixteenth_ms: bpm must be > 0")
    return 15000.0 / bpm


def glide_sixteenths(ms: float, bpm: float) -> float:
    """glide length expressed in sixteenth notes = ms / sixteenth_ms(bpm)"""
    return ms / sixteenth_ms(bpm)


def glide_ms(sixteenths: float, bpm: float) -> float:
    """glide length in ms for a note-division glide = sixteenths * sixteenth_ms(bpm)"""
    return sixteenths * sixteenth_ms(bpm)


def fade_samples(ms: float, sr: int) -> int:
    """whole samples in a fade of at most `ms` ms = floor(ms * sr / 1000)"""
    if ms < 0 or sr <= 0:
        raise ValueError("fade_samples: ms >= 0 and sr > 0 required")
    return int(math.floor(ms * sr / 1000.0))


def cycles_in_window(window_ms: float, freq_hz: float) -> float:
    """cycles = window_ms * freq / 1000. Pitch windows should hold several cycles."""
    return window_ms * freq_hz / 1000.0


def partial_ratio(f_hi: float, f_lo: float) -> float:
    """ratio of two partials = f_hi / f_lo (808 cowbell: 800/540)"""
    if f_lo <= 0:
        raise ValueError("partial_ratio: f_lo must be > 0")
    return f_hi / f_lo


def estimate_pitch_zc(samples, sr: int) -> float:
    """Zero-crossing pitch for a clean, settled, sine-like signal (sub-bass).

    Uses rising crossings with linear interpolation:
    f = (crossings - 1) / (t_last - t_first).
    Cross-check against FFT/YIN before trusting on rich waveforms.
    """
    times = []
    for i in range(1, len(samples)):
        a, b = samples[i - 1], samples[i]
        if a < 0.0 <= b and b != a:
            times.append((i - 1 + (-a) / (b - a)) / sr)
    if len(times) < 3:
        raise ValueError("estimate_pitch_zc: fewer than 3 rising crossings")
    return (len(times) - 1) / (times[-1] - times[0])


def true_peak(samples, taps: int = 16, candidate_floor: float = 0.7, max_candidates: int = 4000) -> float:
    """Approximate true peak by 4x windowed-sinc interpolation near the loudest samples.

    Only samples >= candidate_floor * sample_peak are examined (inter-sample
    overshoot cannot exceed about 3 dB), capped at max_candidates. Approximation:
    use ffmpeg ebur128 or a certified meter for release-critical claims.
    """
    n = len(samples)
    if n == 0:
        return 0.0
    peak = max(abs(s) for s in samples)
    if peak == 0.0:
        return 0.0
    idx = [i for i, s in enumerate(samples) if abs(s) >= candidate_floor * peak]
    idx.sort(key=lambda i: -abs(samples[i]))
    best = peak
    for i in idx[:max_candidates]:
        for frac in (0.25, 0.5, 0.75):
            t = i + frac
            acc = 0.0
            for j in range(max(0, i - taps + 1), min(n, i + taps + 1)):
                d = t - j
                w = 0.5 * (1.0 + math.cos(math.pi * d / taps))
                sinc = 1.0 if d == 0 else math.sin(math.pi * d) / (math.pi * d)
                acc += samples[j] * sinc * w
            best = max(best, abs(acc))
    return best
