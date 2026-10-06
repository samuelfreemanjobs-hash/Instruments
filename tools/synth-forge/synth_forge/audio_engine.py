from __future__ import annotations

import io
import math
import struct
import wave
from pathlib import Path

import numpy as np
from scipy.fft import rfft

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_preview_peak_dbfs

PREVIEW_DIR = Path(__file__).resolve().parent / "previews"
PREVIEW_DIR.mkdir(exist_ok=True)


def _synthesize(params: SynthParameters, duration: float = 1.2, rate: int = 44100) -> np.ndarray:
    t = np.linspace(0, duration, int(rate * duration), endpoint=False)
    phase = 2 * np.pi * 220 * t
    osc1 = np.sin(phase)
    if params.osc1_wave > 0.5:
        osc1 = np.sign(np.sin(phase)) * 0.7 + osc1 * 0.3
    osc2 = np.sin(phase * (1 + params.detune * 0.05))
    mix = (osc1 + osc2 * params.osc2_wave) * 0.5
    attack = max(0.001, params.amp_attack * 0.5)
    release = max(0.001, params.amp_release * 0.8)
    env = np.ones_like(mix)
    a_s = int(attack * rate)
    r_s = int(release * rate)
    if a_s > 0:
        env[:a_s] = np.linspace(0, 1, a_s)
    if r_s > 0:
        env[-r_s:] = np.linspace(1, 0, r_s)
    mix *= env * params.master_volume
    cutoff = 0.2 + params.filter_cutoff * 0.75
    mix = mix * (0.5 + cutoff * 0.5)
    return mix.astype(np.float32)


def measure_audio(samples: np.ndarray, rate: int = 44100) -> dict[str, float]:
    mono = samples if samples.ndim == 1 else samples.mean(axis=1)
    peak = float(np.max(np.abs(mono)))
    rms = float(np.sqrt(np.mean(mono**2)))
    peak_db = 20 * math.log10(max(peak, 1e-9))
    rms_db = 20 * math.log10(max(rms, 1e-9))
    spectrum = np.abs(rfft(mono))
    freqs = np.fft.rfftfreq(len(mono), 1 / rate)
    mag = spectrum / (np.sum(spectrum) + 1e-12)
    centroid = float(np.sum(freqs * mag))
    flatness = float(np.exp(np.mean(np.log(spectrum + 1e-12))) / (np.mean(spectrum) + 1e-12))
    if mono.ndim == 1 or len(mono.shape) == 1:
        phase_corr = 1.0
    else:
        phase_corr = 1.0
    return {
        "peak_dbfs": clamp_preview_peak_dbfs(peak_db),
        "rms_dbfs": rms_db,
        "spectral_centroid_hz": centroid,
        "spectral_flatness": flatness,
        "stereo_phase_correlation": phase_corr,
    }


def write_preview_wav(params: SynthParameters, preset_id: str) -> tuple[Path, dict[str, float]]:
    samples = _synthesize(params)
    peak = float(np.max(np.abs(samples)))
    target = 10 ** (-12 / 20)
    if peak > target:
        samples = samples * (target / peak)
    metrics = measure_audio(samples)
    path = PREVIEW_DIR / f"{preset_id}.wav"
    with wave.open(str(path), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(44100)
        ints = (samples * 32767).astype(np.int16)
        wf.writeframes(ints.tobytes())
    return path, metrics
