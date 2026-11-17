"""Portable DSP calculations for offline QA and reference rendering. Python >=3.10."""
import math


def dbfs(amplitude):
    if amplitude < 0:
        raise ValueError('amplitude must be nonnegative')
    return -math.inf if amplitude == 0 else 20 * math.log10(amplitude)


def amplitude_from_db(db):
    return 10 ** (db / 20)


def hz_to_midi(hz):
    if hz <= 0:
        raise ValueError('hz must be positive')
    return 69 + 12 * math.log2(hz / 440)


def midi_to_hz(midi):
    return 440 * 2 ** ((midi - 69) / 12)


def cents_difference(observed, target):
    if observed <= 0 or target <= 0:
        raise ValueError('frequencies must be positive')
    return 1200 * math.log2(observed / target)


def rms(samples):
    vals = list(samples)
    return math.sqrt(sum(x*x for x in vals) / len(vals)) if vals else 0.0


def estimate_fundamental_autocorrelation(samples, sample_rate, min_hz=30, max_hz=1200):
    """Simple normalized lag correlation for sustained mono waveforms, not a certified pitch tracker.
    Return {'hz': ..., 'confidence': ...}; ambiguous/noisy inputs may give false octave.
    """
    if not samples or sample_rate <= 0 or min_hz <= 0 or max_hz <= min_hz:
        return None
    n = len(samples)
    minlag = max(1, int(sample_rate / max_hz))
    maxlag = min(n // 3, int(sample_rate / min_hz))
    if maxlag <= minlag or n < 3 * minlag:
        return None
    mean = sum(samples) / n
    x = [a - mean for a in samples]
    best_lag, best_score = None, -1.0
    for lag in range(minlag, maxlag + 1):
        # Stride for efficient dependency-free screening; refine near maxima for production.
        step = max(1, (n - lag) // 2500)
        a = b = dot = 0.0
        for i in range(0, n - lag, step):
            u, v = x[i], x[i+lag]
            dot += u*v
            a += u*u
            b += v*v
        score = dot / math.sqrt(a*b) if a > 1e-16 and b > 1e-16 else 0.0
        if score > best_score:
            best_lag, best_score = lag, score
    return {'hz': sample_rate / best_lag, 'confidence': round(best_score, 5)} if best_lag else None
