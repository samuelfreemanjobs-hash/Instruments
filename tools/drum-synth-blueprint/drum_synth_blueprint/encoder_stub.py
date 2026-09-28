"""Optional PyTorch encoder stub: acoustic sample → [pitch_decay, amp_decay, drive]."""

from __future__ import annotations

from typing import TYPE_CHECKING

import numpy as np

from drum_synth_blueprint.mel_loss import mel_spectrogram
from drum_synth_blueprint.synth_808_generator import Synth808Params, synthesize_808_from_vector

if TYPE_CHECKING:
    import torch


def default_param_vector() -> np.ndarray:
    p = Synth808Params()
    return np.array([p.pitch_decay_sec, p.amp_decay_sec, p.drive], dtype=np.float64)


def encode_mel_statistics(wav: np.ndarray, sr: int = 44100) -> np.ndarray:
    """Hand-crafted encoder fallback (no torch): map mel shape → param hints."""
    mel = mel_spectrogram(wav, sr)
    if mel.size == 0:
        return default_param_vector()
    early = mel[:, : min(8, mel.shape[1])].mean()
    late = mel[:, max(0, mel.shape[1] - 8) :].mean()
    brightness = float(np.mean(mel))
    pitch_decay = 0.035 + 0.04 * (early / (late + 1e-6))
    amp_decay = 0.5 + 0.6 * (late / (early + 1e-6))
    drive = 1.1 + 0.5 * min(1.0, brightness / 5.0)
    return np.array(
        [
            float(np.clip(pitch_decay, 0.02, 0.12)),
            float(np.clip(amp_decay, 0.2, 1.5)),
            float(np.clip(drive, 0.8, 2.5)),
        ],
        dtype=np.float64,
    )


def infer_params_numpy(wav: np.ndarray, sr: int = 44100) -> np.ndarray:
    return encode_mel_statistics(wav, sr)


def synthesize_from_wav(wav: np.ndarray, sr: int = 44100) -> tuple[np.ndarray, np.ndarray]:
    """Full numpy pipeline: input hit → params → synthetic 808."""
    params = infer_params_numpy(wav, sr)
    out = synthesize_808_from_vector(float(params[0]), float(params[1]), float(params[2]), sr=sr)
    return params, out


def build_torch_encoder(input_frames: int = 32, n_mels: int = 64):
    """Returns nn.Module mapping [B, n_mels*frames] → [B, 3] when torch installed."""
    try:
        import torch
        import torch.nn as nn
    except ImportError as exc:
        raise RuntimeError("torch required for build_torch_encoder") from exc

    class Encoder808(nn.Module):
        def __init__(self) -> None:
            super().__init__()
            dim = n_mels * input_frames
            self.net = nn.Sequential(
                nn.Linear(dim, 128),
                nn.ReLU(),
                nn.Linear(128, 64),
                nn.ReLU(),
                nn.Linear(64, 3),
            )

        def forward(self, x: "torch.Tensor") -> "torch.Tensor":
            raw = self.net(x)
            pitch_decay = torch.nn.functional.softplus(raw[..., 0:1]) * 0.02 + 0.02
            amp_decay = torch.nn.functional.softplus(raw[..., 1:2]) * 0.3 + 0.2
            drive = torch.nn.functional.softplus(raw[..., 2:3]) * 0.5 + 0.9
            return torch.cat([pitch_decay, amp_decay, drive], dim=-1)

    return Encoder808()
