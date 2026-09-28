"""Waveform encoder → [pitch_decay, amp_decay, drive] for ONNX/JUCE."""

from __future__ import annotations

import torch
import torch.nn as nn

from drum_synth_blueprint.torch_ddsp.synth_torch import Differentiable808Synth


class DDSP808Encoder(nn.Module):
    """
    Maps acoustic drum hit (mono waveform) to synth parameters.

    ONNX input: ``waveform`` float tensor [batch, num_samples]
    ONNX output: ``params`` float tensor [batch, 3]
    """

    def __init__(self, num_samples: int = 8192) -> None:
        super().__init__()
        self.num_samples = num_samples
        self.features = nn.Sequential(
            nn.Conv1d(1, 32, kernel_size=7, stride=2, padding=3),
            nn.ReLU(),
            nn.Conv1d(32, 64, kernel_size=7, stride=2, padding=3),
            nn.ReLU(),
            nn.Conv1d(64, 128, kernel_size=7, stride=2, padding=3),
            nn.ReLU(),
            nn.Conv1d(128, 128, kernel_size=7, stride=2, padding=3),
            nn.ReLU(),
            nn.AdaptiveAvgPool1d(32),
        )
        self.head = nn.Sequential(
            nn.Flatten(),
            nn.Linear(128 * 32, 64),
            nn.ReLU(),
            nn.Linear(64, 3),
        )

    def forward(self, waveform: torch.Tensor) -> torch.Tensor:
        if waveform.dim() == 1:
            waveform = waveform.unsqueeze(0)
        if waveform.dim() == 2:
            waveform = waveform.unsqueeze(1)
        x = waveform[..., : self.num_samples]
        if x.shape[-1] < self.num_samples:
            x = nn.functional.pad(x, (0, self.num_samples - x.shape[-1]))
        raw = self.head(self.features(x))
        pitch_decay = nn.functional.softplus(raw[:, 0:1]) * 0.02 + 0.02
        amp_decay = nn.functional.softplus(raw[:, 1:2]) * 0.3 + 0.2
        drive = nn.functional.softplus(raw[:, 2:3]) * 0.5 + 0.9
        return torch.cat([pitch_decay, amp_decay, drive], dim=1)


class DDSP808TrainableSystem(nn.Module):
    """Encoder + differentiable synth for end-to-end spectral training."""

    def __init__(self, num_samples: int = 8192) -> None:
        super().__init__()
        self.encoder = DDSP808Encoder(num_samples=num_samples)
        self.synth = Differentiable808Synth()

    def forward(self, target_waveform: torch.Tensor) -> tuple[torch.Tensor, torch.Tensor]:
        params = self.encoder(target_waveform)
        synth_wav = self.synth(params)
        return synth_wav, params
