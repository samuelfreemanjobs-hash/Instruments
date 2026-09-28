"""Multi-scale spectral distance (linear + log) — phase-robust vs waveform MSE."""

from __future__ import annotations

import torch
import torch.nn as nn
import torchaudio.transforms as T


class SpectralDistanceLoss(nn.Module):
    """
    Feature difference loss across STFT sizes 512 / 1024 / 2048.

    Normalized Frobenius distance on magnitude spectrograms (linear + log scales).
    """

    def __init__(
        self,
        fft_sizes: tuple[int, ...] = (512, 1024, 2048),
        sample_rate: float = 44100.0,
    ) -> None:
        super().__init__()
        self.sample_rate = sample_rate
        self.transforms = nn.ModuleList(
            [
                T.Spectrogram(n_fft=size, hop_length=max(1, size // 4), power=1.0)
                for size in fft_sizes
            ]
        )

    def forward(self, synthetic: torch.Tensor, target: torch.Tensor) -> torch.Tensor:
        if synthetic.dim() == 1:
            synthetic = synthetic.unsqueeze(0)
        if target.dim() == 1:
            target = target.unsqueeze(0)
        t_len = min(synthetic.shape[-1], target.shape[-1])
        synthetic = synthetic[..., :t_len]
        target = target[..., :t_len]

        total = synthetic.new_zeros(())
        for transform in self.transforms:
            tgt_spec = transform(target) + 1e-7
            syn_spec = transform(synthetic) + 1e-7
            lin = torch.norm(tgt_spec - syn_spec, p="fro") / torch.norm(tgt_spec, p="fro")
            log = torch.norm(torch.log(tgt_spec) - torch.log(syn_spec), p="fro") / torch.norm(
                torch.log(tgt_spec), p="fro"
            )
            total = total + lin + log
        return total / max(len(self.transforms) * 2, 1)
