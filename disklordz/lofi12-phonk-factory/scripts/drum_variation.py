"""Seeded parameter mutation + near-duplicate rejection."""

from __future__ import annotations

import random
from dataclasses import replace
from typing import Callable

from drum_dsp import acoustic_features, feature_distance
from drum_recipe import DrumRecipe, FusionMacros

NEAR_DUP_EPSILON = 0.045


def mutate_fusion(fusion: FusionMacros, rng: random.Random, delta: float = 0.12) -> FusionMacros:
    def n(v: float) -> float:
        return max(0.0, min(1.0, v + rng.uniform(-delta, delta)))

    return FusionMacros(
        memphis_phonk_blend=n(fusion.memphis_phonk_blend),
        drive=n(fusion.drive),
        vintage_character=n(fusion.vintage_character),
        transient_aggression=n(fusion.transient_aggression),
        low_frequency_weight=n(fusion.low_frequency_weight),
        metallic_character=n(fusion.metallic_character),
        density_swing=n(fusion.density_swing),
    )


def recipe_variation(recipe: DrumRecipe, variation_index: int) -> DrumRecipe:
    rng = random.Random(recipe.random_seed + variation_index * 7919)
    vp = dict(recipe.voice_params)
    for key in list(vp.keys()):
        if isinstance(vp[key], (int, float)):
            vp[key] = float(vp[key]) + rng.uniform(-0.08, 0.08)
    return replace(
        recipe,
        fusion=mutate_fusion(recipe.fusion, rng),
        voice_params=vp,
        random_seed=recipe.random_seed + variation_index,
    )


def accept_sample(
    samples: list[float],
    seen_features: list[tuple[float, ...]],
    *,
    epsilon: float = NEAR_DUP_EPSILON,
) -> bool:
    feat = acoustic_features(samples)
    for prev in seen_features:
        if feature_distance(feat, prev) < epsilon:
            return False
    seen_features.append(feat)
    return True


def render_unique(
    render_fn: Callable[[], list[float]],
    seen_features: list[tuple[float, ...]],
    *,
    max_tries: int = 24,
) -> list[float] | None:
    for _ in range(max_tries):
        pcm = render_fn()
        if accept_sample(pcm, seen_features):
            return pcm
    return render_fn()
