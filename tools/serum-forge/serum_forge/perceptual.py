"""Perceptual loss — mel feature difference (see drum-synth-blueprint)."""

from __future__ import annotations

import math


def spectral_flatness(magnitudes: list[float]) -> float:
    if not magnitudes:
        return 0.0
    geo = math.exp(sum(math.log(x + 1e-12) for x in magnitudes) / len(magnitudes))
    arith = sum(magnitudes) / len(magnitudes)
    return geo / (arith + 1e-12)


def feature_difference_loss_paths() -> str:
    """Document where mel loss lives for Serum Forge training hooks."""
    return "tools/drum-synth-blueprint/drum_synth_blueprint/mel_loss.py"


def clap_alignment_score_stub(_audio_path: str, _prompt: str) -> float:
    """Return neutral score when LAION-CLAP is not installed."""
    return 0.0
