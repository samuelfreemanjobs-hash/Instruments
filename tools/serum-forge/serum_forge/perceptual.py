"""Perceptual loss stubs (extend with torchaudio / CLAP in production)."""

from __future__ import annotations

import math


def spectral_flatness(magnitudes: list[float]) -> float:
    if not magnitudes:
        return 0.0
    geo = math.exp(sum(math.log(x + 1e-12) for x in magnitudes) / len(magnitudes))
    arith = sum(magnitudes) / len(magnitudes)
    return geo / (arith + 1e-12)


def clap_alignment_score_stub(_audio_path: str, _prompt: str) -> float:
    """Return neutral score when LAION-CLAP is not installed."""
    return 0.0
