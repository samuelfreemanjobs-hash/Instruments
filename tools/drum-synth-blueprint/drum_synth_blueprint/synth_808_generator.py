#!/usr/bin/env python3
"""
Pure math 808 / phonk core — continuous exponential pitch decay + tanh saturation.

Models analog-style pitch droop (e.g. 350 Hz → 45 Hz) via integrated phase, not
per-sample linear frequency dots.
"""

from __future__ import annotations

from dataclasses import dataclass

import numpy as np

SR = 44100


@dataclass(frozen=True)
class Synth808Params:
    """DDSP / ONNX export parameter vector (3 primary controls)."""

    pitch_decay_sec: float = 0.055
    amp_decay_sec: float = 0.85
    drive: float = 1.35
    pitch_start_hz: float = 350.0
    pitch_end_hz: float = 45.0
    duration_sec: float = 1.6
    sr: int = SR


def exponential_pitch_hz(t: np.ndarray, p: Synth808Params) -> np.ndarray:
    """Pitch curve: f(t) = f_end + (f_start - f_end) * exp(-t / tau)."""
    tau = max(1e-4, p.pitch_decay_sec)
    return p.pitch_end_hz + (p.pitch_start_hz - p.pitch_end_hz) * np.exp(-t / tau)


def exponential_amp_envelope(n: int, p: Synth808Params) -> np.ndarray:
    """Amplitude: fast attack + exponential decay (circuit-style, not ADSR stairs)."""
    t = np.arange(n, dtype=np.float64) / p.sr
    attack = 0.002
    decay_tau = max(1e-3, p.amp_decay_sec)
    env = np.where(t < attack, t / attack, np.exp(-(t - attack) / decay_tau))
    gate = p.duration_sec
    env = np.where(t > gate, env * np.exp(-(t - gate) / 0.25), env)
    return env


def synthesize_808(params: Synth808Params | None = None) -> np.ndarray:
    """
    Drift-phonk style 808: sine body + light harmonics, np.tanh wave-shaping.

    Phase integrates instantaneous frequency each sample (continuous interpolation).
    """
    p = params or Synth808Params()
    n = int(p.sr * (p.duration_sec + 0.15))
    t = np.arange(n, dtype=np.float64) / p.sr

    freq = exponential_pitch_hz(t, p)
    phase_inc = 2.0 * np.pi * freq / p.sr
    phase = np.cumsum(phase_inc)

    body = np.sin(phase)
    harmonics = 0.18 * np.sin(phase * 2.0) + 0.06 * np.sin(phase * 3.0)
    raw = body + harmonics

    shaped = np.tanh(raw * p.drive)
    return shaped * exponential_amp_envelope(n, p)


def synthesize_808_from_vector(
    pitch_decay: float,
    amp_decay: float,
    drive: float,
    *,
    pitch_start_hz: float = 350.0,
    pitch_end_hz: float = 45.0,
    duration_sec: float = 1.6,
    sr: int = SR,
) -> np.ndarray:
    """Encoder network output layout → waveform (training / ONNX parity)."""
    return synthesize_808(
        Synth808Params(
            pitch_decay_sec=pitch_decay,
            amp_decay_sec=amp_decay,
            drive=drive,
            pitch_start_hz=pitch_start_hz,
            pitch_end_hz=pitch_end_hz,
            duration_sec=duration_sec,
            sr=sr,
        )
    )


def normalize_peak(samples: np.ndarray, peak: float = 0.95) -> np.ndarray:
    m = float(np.max(np.abs(samples))) or 1e-9
    return (samples / m) * peak


if __name__ == "__main__":
    wav = normalize_peak(synthesize_808())
    print("synth_808_generator: ok", len(wav), float(np.max(np.abs(wav))))
