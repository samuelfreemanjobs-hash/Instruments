"""Mel-spectrogram feature difference loss (phase-robust training target)."""

from __future__ import annotations

import numpy as np

SR = 44100
N_FFT = 1024
HOP = 256
N_MELS = 64
FMIN = 30.0
FMAX = 8000.0


def _hz_to_mel(hz: float) -> float:
    return 2595.0 * np.log10(1.0 + hz / 700.0)


def _mel_to_hz(mel: float) -> float:
    return 700.0 * (10.0 ** (mel / 2595.0) - 1.0)


def mel_filterbank(sr: int = SR, n_fft: int = N_FFT, n_mels: int = N_MELS) -> np.ndarray:
    """Triangular mel filterbank (Hz → mel bins)."""
    fft_freqs = np.linspace(0, sr / 2, n_fft // 2 + 1)
    mel_min = _hz_to_mel(FMIN)
    mel_max = _hz_to_mel(min(FMAX, sr / 2))
    mel_pts = np.linspace(mel_min, mel_max, n_mels + 2)
    hz_pts = _mel_to_hz(mel_pts)
    fb = np.zeros((n_mels, len(fft_freqs)), dtype=np.float64)
    for i in range(n_mels):
        left, center, right = hz_pts[i], hz_pts[i + 1], hz_pts[i + 2]
        for j, f in enumerate(fft_freqs):
            if left <= f <= center:
                fb[i, j] = (f - left) / max(center - left, 1e-9)
            elif center < f <= right:
                fb[i, j] = (right - f) / max(right - center, 1e-9)
    return fb


def mel_spectrogram(mono: np.ndarray, sr: int = SR) -> np.ndarray:
    """Power mel spectrogram [n_mels, frames]."""
    if mono.size < N_FFT:
        mono = np.pad(mono, (0, N_FFT - mono.size))
    window = np.hanning(N_FFT)
    fb = mel_filterbank(sr=sr)
    frames = []
    for start in range(0, len(mono) - N_FFT, HOP):
        frame = mono[start : start + N_FFT] * window
        spec = np.abs(np.fft.rfft(frame)) ** 2
        mel = fb @ spec[: fb.shape[1]]
        frames.append(mel)
    if not frames:
        return np.zeros((N_MELS, 1), dtype=np.float64)
    out = np.stack(frames, axis=1)
    return np.log1p(out)


def feature_difference_loss(target: np.ndarray, synthetic: np.ndarray, sr: int = SR) -> float:
    """
    L2 distance between log-mel features (ignores waveform phase misalignment).

    Returns scalar loss; lower is better timbre match.
    """
    n = min(len(target), len(synthetic))
    if n < N_FFT:
        return float("inf")
    mt = mel_spectrogram(target[:n], sr)
    ms = mel_spectrogram(synthetic[:n], sr)
    cols = min(mt.shape[1], ms.shape[1])
    if cols == 0:
        return float("inf")
    diff = mt[:, :cols] - ms[:, :cols]
    return float(np.mean(diff * diff))


def loss_for_synth_params(
    target_wav: np.ndarray,
    pitch_decay: float,
    amp_decay: float,
    drive: float,
    *,
    sr: int = SR,
) -> float:
    from drum_synth_blueprint.synth_808_generator import synthesize_808_from_vector

    syn = synthesize_808_from_vector(pitch_decay, amp_decay, drive, sr=sr)
    return feature_difference_loss(target_wav, syn, sr)
