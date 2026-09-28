"""Differentiable 808 synthesizer — pure torch.Tensor ops for backprop."""

from __future__ import annotations

import math

import torch
import torch.nn as nn


class Differentiable808Synth(nn.Module):
    """
    Maps normalized control vector ``[0, 1]^4`` → mono waveform.

    Columns: pitch_decay_rate, amp_decay_rate, drive, base_freq (see ``forward`` denorm).
    Pitch sweep: ``base_freq + 300 * exp(-pitch_decay * t)`` with integrated phase.
    """

    def __init__(
        self,
        sample_rate: float = 44100.0,
        duration_sec: float = 2.0,
    ) -> None:
        super().__init__()
        self.sample_rate = sample_rate
        self.duration_sec = duration_sec
        self.n_samples = int(sample_rate * duration_sec)

    def forward(self, params: torch.Tensor) -> torch.Tensor:
        if params.dim() == 1:
            params = params.unsqueeze(0)
        batch, device, dtype = params.shape[0], params.device, params.dtype
        n = self.n_samples
        t = torch.arange(n, device=device, dtype=dtype) / self.sample_rate
        t = t.unsqueeze(0).expand(batch, -1)

        pitch_decay = 10.0 + params[:, 0:1] * 90.0
        amp_decay = 1.0 + params[:, 1:2] * 9.0
        drive = 0.1 + params[:, 2:3] * 4.9
        base_freq = 40.0 + params[:, 3:4] * 25.0

        pitch_env = torch.exp(-pitch_decay * t)
        freq_curve = base_freq + 300.0 * pitch_env
        phase = (2.0 * math.pi) * torch.cumsum(freq_curve / self.sample_rate, dim=1)
        raw_sine = torch.sin(phase)

        amp_env = torch.exp(-amp_decay * t)
        shaped = torch.tanh(raw_sine * drive)
        return shaped * amp_env
