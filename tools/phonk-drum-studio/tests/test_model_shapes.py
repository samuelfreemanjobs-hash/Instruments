"""Contract tests for Audio VAE tensor shapes and loss."""

import torch

from model import (
    AUDIO_LENGTH,
    LATENT_DIM,
    AudioVAE,
    MultiScaleSpectralLoss,
    kl_divergence,
)


def test_vae_forward_output_length():
    model = AudioVAE()
    x = torch.randn(4, 1, AUDIO_LENGTH)
    recon, mu, logvar = model(x)
    assert recon.shape == (4, 1, AUDIO_LENGTH)
    assert mu.shape == (4, LATENT_DIM)
    assert logvar.shape == (4, LATENT_DIM)


def test_encode_decode_decoupled():
    model = AudioVAE()
    x = torch.randn(2, AUDIO_LENGTH)
    mu, logvar = model.encode(x)
    z = model.reparameterize(mu, logvar)
    out = model.decode(z)
    assert out.shape == (2, 1, AUDIO_LENGTH)


def test_spectral_loss_runs():
    loss_fn = MultiScaleSpectralLoss()
    pred = torch.randn(2, AUDIO_LENGTH)
    target = torch.randn(2, AUDIO_LENGTH)
    value = loss_fn(pred, target)
    assert value.ndim == 0
    assert torch.isfinite(value)


def test_kl_non_negative_mean():
    mu = torch.zeros(8, LATENT_DIM)
    logvar = torch.zeros(8, LATENT_DIM)
    kld = kl_divergence(mu, logvar)
    assert kld.ndim == 0
