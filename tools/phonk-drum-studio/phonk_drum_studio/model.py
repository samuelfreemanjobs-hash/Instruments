"""Audio VAE and multi-scale spectral loss for Phonk Drum Studio."""

from __future__ import annotations

from typing import List, Sequence, Tuple

import torch
import torch.nn as nn
import torch.nn.functional as F

SAMPLE_RATE = 44_100
AUDIO_LENGTH = 22_050
LATENT_DIM = 64


def _conv_out_length(length: int, kernel: int, stride: int, padding: int) -> int:
    return (length + 2 * padding - kernel) // stride + 1


def _conv_transpose_out_length(
    length: int, kernel: int, stride: int, padding: int, output_padding: int = 0
) -> int:
    return (length - 1) * stride - 2 * padding + kernel + output_padding


class MultiScaleSpectralLoss(nn.Module):
    """L1 linear- and log-magnitude STFT loss at multiple scales."""

    def __init__(
        self,
        fft_sizes: Sequence[int] = (256, 512, 1024),
        hop_sizes: Sequence[int] = (64, 128, 256),
        eps: float = 1e-7,
    ) -> None:
        super().__init__()
        if len(fft_sizes) != len(hop_sizes):
            raise ValueError("fft_sizes and hop_sizes must have the same length")
        self.scales: List[Tuple[int, int]] = list(zip(fft_sizes, hop_sizes))
        self.eps = eps

    def _stft_mag(self, x: torch.Tensor, n_fft: int, hop_length: int) -> torch.Tensor:
        window = torch.hann_window(n_fft, device=x.device, dtype=x.dtype)
        spec = torch.stft(
            x,
            n_fft=n_fft,
            hop_length=hop_length,
            win_length=n_fft,
            window=window,
            center=True,
            pad_mode="reflect",
            normalized=False,
            onesided=True,
            return_complex=True,
        )
        return spec.abs()

    def forward(self, pred: torch.Tensor, target: torch.Tensor) -> torch.Tensor:
        if pred.dim() == 3:
            pred = pred.squeeze(1)
        if target.dim() == 3:
            target = target.squeeze(1)
        if pred.shape != target.shape:
            min_len = min(pred.shape[-1], target.shape[-1])
            pred = pred[..., :min_len]
            target = target[..., :min_len]

        loss = pred.new_zeros(())
        for n_fft, hop in self.scales:
            mag_p = self._stft_mag(pred, n_fft, hop)
            mag_t = self._stft_mag(target, n_fft, hop)
            min_f = min(mag_p.shape[-2], mag_t.shape[-2])
            min_t = min(mag_p.shape[-1], mag_t.shape[-1])
            mag_p = mag_p[..., :min_f, :min_t]
            mag_t = mag_t[..., :min_f, :min_t]

            loss = loss + F.l1_loss(mag_p, mag_t)
            loss = loss + F.l1_loss(torch.log(mag_p + self.eps), torch.log(mag_t + self.eps))

        return loss / len(self.scales)


class AudioVAE(nn.Module):
    """
    1D convolutional VAE for fixed-length drum one-shots (22,050 samples @ 44.1 kHz).
    """

    def __init__(
        self,
        audio_length: int = AUDIO_LENGTH,
        latent_dim: int = LATENT_DIM,
        channels: Sequence[int] = (1, 32, 64, 128, 256, 256, 256, 256, 256),
    ) -> None:
        super().__init__()
        self.audio_length = audio_length
        self.latent_dim = latent_dim
        self.channel_schedule = list(channels)

        enc_layers: List[nn.Module] = []
        length = audio_length
        for in_ch, out_ch in zip(self.channel_schedule[:-1], self.channel_schedule[1:]):
            enc_layers.append(nn.Conv1d(in_ch, out_ch, kernel_size=4, stride=2, padding=1))
            enc_layers.append(nn.LeakyReLU(0.2, inplace=True))
            length = _conv_out_length(length, 4, 2, 1)
        self.encoder_convs = nn.Sequential(*enc_layers)
        self.bottleneck_length = length
        self.bottleneck_channels = self.channel_schedule[-1]

        self.enc_pool = nn.AdaptiveAvgPool1d(1)
        self.fc_mu = nn.Linear(self.bottleneck_channels, latent_dim)
        self.fc_logvar = nn.Linear(self.bottleneck_channels, latent_dim)

        self.dec_fc = nn.Linear(latent_dim, self.bottleneck_channels * self.bottleneck_length)

        dec_schedule = list(reversed(self.channel_schedule))
        dec_layers: List[nn.Module] = []
        dec_len = self.bottleneck_length
        for in_ch, out_ch in zip(dec_schedule[:-1], dec_schedule[1:]):
            out_pad = 0
            next_len = _conv_transpose_out_length(dec_len, 4, 2, 1, out_pad)
            dec_layers.append(
                nn.ConvTranspose1d(in_ch, out_ch, kernel_size=4, stride=2, padding=1, output_padding=out_pad)
            )
            dec_layers.append(nn.LeakyReLU(0.2, inplace=True))
            dec_len = next_len
        self.decoder_convs = nn.Sequential(*dec_layers)
        self.decoder_raw_length = dec_len
        self.out_conv = nn.Conv1d(dec_schedule[-1], 1, kernel_size=7, padding=3)

    def _fit_length(self, x: torch.Tensor) -> torch.Tensor:
        t = x.shape[-1]
        if t == self.audio_length:
            return x
        if t > self.audio_length:
            start = (t - self.audio_length) // 2
            return x[..., start : start + self.audio_length]
        return F.pad(x, (0, self.audio_length - t))

    def encode(self, x: torch.Tensor) -> Tuple[torch.Tensor, torch.Tensor]:
        if x.dim() == 2:
            x = x.unsqueeze(1)
        h = self.encoder_convs(x)
        h = self.enc_pool(h).squeeze(-1)
        return self.fc_mu(h), self.fc_logvar(h)

    def reparameterize(self, mu: torch.Tensor, logvar: torch.Tensor) -> torch.Tensor:
        if self.training:
            std = torch.exp(0.5 * logvar)
            return mu + torch.randn_like(std) * std
        return mu

    def decode(self, z: torch.Tensor) -> torch.Tensor:
        h = self.dec_fc(z)
        h = h.view(z.shape[0], self.bottleneck_channels, self.bottleneck_length)
        h = self.decoder_convs(h)
        h = torch.tanh(self.out_conv(h))
        return self._fit_length(h)

    def forward(self, x: torch.Tensor) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
        mu, logvar = self.encode(x)
        z = self.reparameterize(mu, logvar)
        return self.decode(z), mu, logvar


def kl_divergence(mu: torch.Tensor, logvar: torch.Tensor) -> torch.Tensor:
    return -0.5 * torch.mean(1 + logvar - mu.pow(2) - logvar.exp())


def peak_normalize(waveform: torch.Tensor, eps: float = 1e-8) -> torch.Tensor:
    peak = waveform.abs().amax(dim=-1, keepdim=True).clamp_min(eps)
    return (waveform / peak).clamp(-1.0, 1.0)
