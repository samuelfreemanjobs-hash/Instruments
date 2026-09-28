"""Differentiable 808 synthesizer — pure torch.Tensor ops for backprop."""

from __future__ import annotations

import math

import torch
import torch.nn as nn


class Differentiable808Synth(nn.Module):
    """
    Serum-style control vector → mono waveform.

    params[:, 0] pitch_decay_sec
    params[:, 1] amp_decay_sec
    params[:, 2] drive (tanh pre-gain)
    Fixed phonk sweep 350 Hz → 45 Hz (matches numpy blueprint).
    """

    def __init__(
        self,
        sample_rate: float = 44100.0,
        duration_sec: float = 1.6,
        pitch_start_hz: float = 350.0,
        pitch_end_hz: float = 45.0,
    ) -> None:
        super().__init__()
        self.sample_rate = sample_rate
        self.duration_sec = duration_sec
        self.pitch_start_hz = pitch_start_hz
        self.pitch_end_hz = pitch_end_hz
        self.n_samples = int(sample_rate * (duration_sec + 0.15))

    def forward(self, params: torch.Tensor) -> torch.Tensor:
        if params.dim() == 1:
            params = params.unsqueeze(0)
        batch, device, dtype = params.shape[0], params.device, params.dtype
        n = self.n_samples
        t = torch.arange(n, device=device, dtype=dtype) / self.sample_rate
        t = t.unsqueeze(0).expand(batch, -1)

        pitch_tau = params[:, 0:1].clamp(min=1e-4)
        amp_tau = params[:, 1:2].clamp(min=1e-3)
        drive = params[:, 2:3].clamp(min=0.1, max=4.0)

        freq = self.pitch_end_hz + (self.pitch_start_hz - self.pitch_end_hz) * torch.exp(-t / pitch_tau)
        phase_inc = (2.0 * math.pi) * freq / self.sample_rate
        phase = torch.cumsum(phase_inc, dim=1)

        body = torch.sin(phase)
        harm = 0.18 * torch.sin(phase * 2.0) + 0.06 * torch.sin(phase * 3.0)
        raw = body + harm

        attack = 0.002
        env = torch.where(
            t < attack,
            t / attack,
            torch.exp(-(t - attack) / amp_tau),
        )
        gate = self.duration_sec
        env = torch.where(t > gate, env * torch.exp(-(t - gate) / 0.25), env)

        shaped = torch.tanh(raw * drive)
        return shaped * env
