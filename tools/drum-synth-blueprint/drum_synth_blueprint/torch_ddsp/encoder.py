"""Mel encoder → four normalized synth controls; full trainable DDSP stack."""

from __future__ import annotations

import torch
import torch.nn as nn

from drum_synth_blueprint.torch_ddsp.mel_front_end import MEL_N_MELS, make_mel_spectrogram
from drum_synth_blueprint.torch_ddsp.synth_torch import Differentiable808Synth


class DDSP808Encoder(nn.Module):
    """
    Mel-spectrogram → bounded synthesis parameters.

    ONNX input: ``mel_input`` float tensor ``[batch, 1, n_mels, time_frames]``
    ONNX output: ``synth_parameters`` float tensor ``[batch, 4]`` in ``[0, 1]``
    """

    def __init__(self, n_mels: int = MEL_N_MELS) -> None:
        super().__init__()
        self.n_mels = n_mels
        self.conv_stack = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1),
            nn.BatchNorm2d(16),
            nn.ReLU(),
            nn.Conv2d(16, 32, kernel_size=3, stride=2, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=3, stride=2, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(),
            nn.AdaptiveAvgPool2d((4, 4)),
        )
        self.fc_out = nn.Sequential(
            nn.Linear(64 * 4 * 4, 128),
            nn.ReLU(),
            nn.Linear(128, 4),
            nn.Sigmoid(),
        )

    def forward(self, mel: torch.Tensor) -> torch.Tensor:
        if mel.dim() == 3:
            mel = mel.unsqueeze(1)
        x = self.conv_stack(mel)
        x = x.view(x.size(0), -1)
        return self.fc_out(x)


class DDSP808TrainableSystem(nn.Module):
    """Mel front-end + encoder + differentiable synth (end-to-end training)."""

    def __init__(
        self,
        *,
        sample_rate: float = 44100.0,
        duration_sec: float = 2.0,
        n_mels: int = MEL_N_MELS,
    ) -> None:
        super().__init__()
        self.sample_rate = sample_rate
        self.mel_transform = make_mel_spectrogram(sample_rate=int(sample_rate), n_mels=n_mels)
        self.encoder = DDSP808Encoder(n_mels=n_mels)
        self.synth = Differentiable808Synth(sample_rate=sample_rate, duration_sec=duration_sec)

    def forward(self, target_waveform: torch.Tensor) -> tuple[torch.Tensor, torch.Tensor]:
        if target_waveform.dim() == 1:
            target_waveform = target_waveform.unsqueeze(0)
        mel = self.mel_transform(target_waveform).unsqueeze(1)
        params = self.encoder(mel)
        synth_wav = self.synth(params)
        n = min(target_waveform.shape[-1], synth_wav.shape[-1])
        return synth_wav[..., :n], params

    def mel_features(self, waveform: torch.Tensor) -> torch.Tensor:
        if waveform.dim() == 1:
            waveform = waveform.unsqueeze(0)
        return self.mel_transform(waveform).unsqueeze(1)
