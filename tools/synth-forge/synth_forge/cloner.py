from __future__ import annotations

import io
import math
import struct
import wave
from typing import Any

import numpy as np
from scipy.fft import rfft, rfftfreq
from scipy.signal import find_peaks

from synth_forge.generator import generate_batch
from synth_forge.models import CloneAudioRequest, Preset, SynthParameters
from synth_forge.safety import clamp_for_hardware

NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]


def _load_wav_mono(data: bytes) -> tuple[np.ndarray, int]:
    with wave.open(io.BytesIO(data), "rb") as wf:
        rate = wf.getframerate()
        frames = wf.readframes(wf.getnframes())
        channels = wf.getnchannels()
        width = wf.getsampwidth()
    if width == 2:
        samples = np.frombuffer(frames, dtype=np.int16).astype(np.float32) / 32768.0
    elif width == 4:
        samples = np.frombuffer(frames, dtype=np.int32).astype(np.float32) / 2147483648.0
    else:
        raise ValueError("unsupported WAV sample width")
    if channels > 1:
        samples = samples.reshape(-1, channels).mean(axis=1)
    return samples, rate


def _rms_envelope(samples: np.ndarray, rate: int) -> dict[str, float]:
    window = max(1, rate // 100)
    env = np.convolve(samples**2, np.ones(window) / window, mode="same")
    env = np.sqrt(env)
    peak = float(np.max(env)) or 1e-9
    norm = env / peak
    thresh = 0.2
    active = norm > thresh
    if not np.any(active):
        return {"attack": 0.1, "decay": 0.3, "sustain": 0.5, "release": 0.4}
    idx = np.where(active)[0]
    attack_i = idx[0]
    release_i = idx[-1]
    sustain_level = float(np.median(norm[attack_i:release_i]))
    return {
        "attack": min(1.0, attack_i / rate),
        "decay": 0.25,
        "sustain": sustain_level,
        "release": min(1.0, (len(samples) - release_i) / rate),
    }


def classify_wave(magnitudes: np.ndarray) -> str:
    if magnitudes.size < 4:
        return "sine"
    har = magnitudes[1:8]
    har = har / (np.max(har) + 1e-12)
    odd = float(np.mean(har[0::2]))
    even = float(np.mean(har[1::2])) if len(har) > 1 else 0.0
    if odd > 0.6 and even < 0.35:
        return "square"
    if har[0] > 0.7 and np.sum(har[1:]) > 1.2:
        return "saw"
    return "triangle_sine"


def detect_pitch(samples: np.ndarray, rate: int) -> tuple[float, str]:
    if len(samples) < rate // 4:
        return 440.0, "A4"
    spectrum = np.abs(rfft(samples * np.hanning(len(samples))))
    freqs = rfftfreq(len(samples), 1 / rate)
    peaks, _ = find_peaks(spectrum, height=np.max(spectrum) * 0.2)
    if len(peaks) == 0:
        return 440.0, "A4"
    f0 = float(freqs[peaks[0]])
    if f0 <= 0:
        return 440.0, "A4"
    midi = 69 + 12 * math.log2(f0 / 440.0)
    note = NOTE_NAMES[int(round(midi)) % 12]
    octave = int(round(midi)) // 12 - 1
    return f0, f"{note}{octave}"


def analyze_wav(data: bytes) -> dict[str, Any]:
    samples, rate = _load_wav_mono(data)
    adsr = _rms_envelope(samples, rate)
    spectrum = np.abs(rfft(samples * np.hanning(len(samples))))
    wave_shape = classify_wave(spectrum)
    f0, note = detect_pitch(samples, rate)
    return {
        "f0_hz": f0,
        "note": note,
        "wave_shape": wave_shape,
        "adsr": adsr,
        "peak_dbfs": 20 * math.log10(max(float(np.max(np.abs(samples))), 1e-9)),
    }


def analysis_to_parameters(analysis: dict[str, Any]) -> SynthParameters:
    adsr = analysis["adsr"]
    shape = analysis["wave_shape"]
    osc1 = {"sine": 0.05, "triangle_sine": 0.25, "saw": 0.9, "square": 0.75}.get(shape, 0.5)
    params = SynthParameters(
        osc1_wave=osc1,
        osc2_wave=min(1.0, osc1 + 0.1),
        amp_attack=adsr["attack"],
        amp_decay=adsr["decay"],
        amp_sustain=adsr["sustain"],
        amp_release=adsr["release"],
        filter_cutoff=0.55 if shape == "sine" else 0.68,
        category="cloned",
    )
    return clamp_for_hardware(params)


def clone_from_wav(data: bytes, req: CloneAudioRequest) -> list[Preset]:
    analysis = analyze_wav(data)
    seed = analysis_to_parameters(analysis)
    presets = generate_batch(req.synth_id, req.count, category="cloned", parent=seed)
    for p in presets:
        p.name = f"clone_{analysis['note']}_{p.name}"
    return presets
