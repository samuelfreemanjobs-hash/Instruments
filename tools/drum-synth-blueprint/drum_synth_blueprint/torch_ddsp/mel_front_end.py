"""Shared mel spectrogram settings for training, inference, and JUCE parity."""

from __future__ import annotations

import torchaudio.transforms as T

MEL_N_FFT = 2048
MEL_HOP_LENGTH = 1024
MEL_N_MELS = 128


def make_mel_spectrogram(*, sample_rate: int = 44100, n_mels: int = MEL_N_MELS) -> T.MelSpectrogram:
    return T.MelSpectrogram(
        sample_rate=sample_rate,
        n_fft=MEL_N_FFT,
        hop_length=MEL_HOP_LENGTH,
        n_mels=n_mels,
    )
