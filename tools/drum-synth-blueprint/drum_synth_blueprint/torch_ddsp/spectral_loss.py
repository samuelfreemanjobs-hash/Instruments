"""Multi-scale mel spectral distance (linear + log) — phase-robust vs waveform MSE."""

from __future__ import annotations

import math

import torch
import torch.nn as nn
import torch.nn.functional as F


def _mel_filterbank(n_fft: int, n_mels: int, sr: float, device: torch.device, dtype: torch.dtype) -> torch.Tensor:
    fmin, fmax = 30.0, min(8000.0, sr / 2)
    mel_min = 2595.0 * math.log10(1.0 + fmin / 700.0)
    mel_max = 2595.0 * math.log10(1.0 + fmax / 700.0)
    mels = torch.linspace(mel_min, mel_max, n_mels + 2, device=device, dtype=dtype)
    hz = 700.0 * (10.0 ** (mels / 2595.0) - 1.0)
    fft_freqs = torch.linspace(0, sr / 2, n_fft // 2 + 1, device=device, dtype=dtype)
    fb = torch.zeros(n_mels, fft_freqs.numel(), device=device, dtype=dtype)
    for i in range(n_mels):
        left, center, right = hz[i], hz[i + 1], hz[i + 2]
        up = (fft_freqs - left) / (center - left + 1e-9)
        down = (right - fft_freqs) / (right - center + 1e-9)
        fb[i] = torch.clamp(torch.min(up, down), min=0.0)
    return fb


def _mel_linear_power(wav: torch.Tensor, n_fft: int, hop: int, sr: float, n_mels: int) -> torch.Tensor:
    """wav [B, T] -> mel power [B, n_mels, frames] (linear scale)."""
    window = torch.hann_window(n_fft, device=wav.device, dtype=wav.dtype)
    spec = torch.stft(
        wav,
        n_fft=n_fft,
        hop_length=hop,
        win_length=n_fft,
        window=window,
        center=True,
        return_complex=True,
    )
    power = spec.abs().pow(2)
    fb = _mel_filterbank(n_fft, n_mels, sr, wav.device, wav.dtype)
    return torch.matmul(fb, power)


class SpectralDistanceLoss(nn.Module):
    """
    Multi-scale mel distance across FFT 512 / 1024 / 2048.

    Each scale contributes linear mel MSE + log-mel MSE (transients + sub tails).
    """

    def __init__(
        self,
        fft_sizes: tuple[int, ...] = (512, 1024, 2048),
        hop_ratio: float = 0.25,
        n_mels: int = 64,
        sample_rate: float = 44100.0,
    ) -> None:
        super().__init__()
        self.fft_sizes = fft_sizes
        self.hop_ratio = hop_ratio
        self.n_mels = n_mels
        self.sample_rate = sample_rate

    def forward(self, synthetic: torch.Tensor, target: torch.Tensor) -> torch.Tensor:
        if synthetic.dim() == 1:
            synthetic = synthetic.unsqueeze(0)
        if target.dim() == 1:
            target = target.unsqueeze(0)
        t_len = min(synthetic.shape[-1], target.shape[-1])
        synthetic = synthetic[..., :t_len]
        target = target[..., :t_len]

        total = synthetic.new_zeros(())
        n_terms = 0
        for n_fft in self.fft_sizes:
            hop = max(1, int(n_fft * self.hop_ratio))
            mel_syn = _mel_linear_power(synthetic, n_fft, hop, self.sample_rate, self.n_mels)
            mel_tgt = _mel_linear_power(target, n_fft, hop, self.sample_rate, self.n_mels)
            cols = min(mel_syn.shape[-1], mel_tgt.shape[-1])
            mel_syn = mel_syn[..., :cols]
            mel_tgt = mel_tgt[..., :cols]
            lin = F.mse_loss(mel_syn, mel_tgt)
            log = F.mse_loss(torch.log1p(mel_syn), torch.log1p(mel_tgt))
            total = total + lin + log
            n_terms += 2
        return total / max(n_terms, 1)
