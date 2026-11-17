"""Dependency-free pitch cross-check for sustained pitched drum tails.

Checks autocorrelation and a windowed direct Fourier search around the target;
does NOT claim certified YIN/FFT, and is unreliable for inharmonic/noisy hits.
"""
import math
import wave
import struct
from dsp_math import cents_difference, estimate_fundamental_autocorrelation


def read_pcm16_mono(path):
    with wave.open(str(path), 'rb') as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 2 or wav.getcomptype() != 'NONE':
            raise ValueError('pitch cross-check requires mono PCM16 WAV')
        rate = wav.getframerate()
        data = wav.readframes(wav.getnframes())
    return [v[0] / 32768 for v in struct.iter_unpack('<h', data)], rate


def windowed_spectral_peak(samples, rate, target, cents_range=100):
    """Target-centered Goertzel-like direct projections; spectral peak may be a harmonic."""
    n = min(len(samples), 4096)
    if n < 128:
        return None
    x = samples[-n:]
    # remove DC and apply Hann; perform a fine grid in cents, not FFT bins.
    dc = sum(x)/n
    x = [(v-dc)*(0.5-0.5*math.cos(2*math.pi*i/(n-1))) for i,v in enumerate(x)]
    best = (-1.0, None)
    for step in range(-cents_range, cents_range+1, 5):
        hz = target * 2**(step/1200)
        angle = 2*math.pi*hz/rate
        re = im = 0.0
        for i,v in enumerate(x):
            re += v*math.cos(angle*i)
            im += v*math.sin(angle*i)
        power = re*re+im*im
        if power > best[0]:
            best = (power,hz)
    return best[1]


def inspect_pitch(path, target_hz, tolerance_cents=35):
    if not isinstance(target_hz, (int,float)) or not math.isfinite(target_hz) or target_hz <= 0:
        raise ValueError('invalid target_hz')
    audio, rate = read_pcm16_mono(path)
    # Avoid the transient/pitch sweep and too-quiet final samples.
    start = int(rate * 0.12)
    end = min(len(audio), start + int(rate*0.30))
    tail = audio[start:end]
    if len(tail) < 256 or max(map(abs,tail), default=0) < 0.0008:
        return {'status':'INCONCLUSIVE','reason':'insufficient sustained pitched tail'}
    # Strided data reduces the quadratic autocorrelation cost on 44.1k audio.
    stride = max(1, rate//6000)
    lowrate = rate/stride
    down = tail[::stride]
    auto = estimate_fundamental_autocorrelation(down, lowrate, max(20,target_hz/1.5), min(lowrate/3,target_hz*1.5))
    spec = windowed_spectral_peak(tail, rate, target_hz)
    if auto is None or spec is None or auto['confidence'] < 0.65:
        return {'status':'INCONCLUSIVE','autocorrelation':auto,'spectral_hz':spec}
    aerr, serr = cents_difference(auto['hz'],target_hz), cents_difference(spec,target_hz)
    return {'status':'PASS' if abs(aerr)<=tolerance_cents and abs(serr)<=tolerance_cents else 'FAIL',
            'target_hz':target_hz, 'autocorrelation_hz':round(auto['hz'],3),
            'autocorrelation_confidence':auto['confidence'], 'spectral_hz':round(spec,3),
            'autocorrelation_error_cents':round(aerr,2),'spectral_error_cents':round(serr,2),
            'tolerance_cents':tolerance_cents}
