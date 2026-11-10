"""Apply fusion macros to synthesis params and post chains."""

from __future__ import annotations

from dataclasses import replace

from drum_dsp import (
    apply_saturation,
    bit_depth_reduce,
    low_shelf_boost,
    zero_order_hold,
)
from drum_recipe import FusionMacros
from factory_modes import FactoryModeProfile
from phonk_machine_engine import ENGINE_LEGACY_ALIASES, resolve_engine_id
from phonk_synth import PhonkParams, resolve_phonk_params


def resolve_engine(engine: str | None) -> str:
    if not engine:
        return "hard_dirt_808"
    key = engine.strip().lower().replace("-", "_")
    if key in ENGINE_LEGACY_ALIASES:
        return ENGINE_LEGACY_ALIASES[key]
    return resolve_engine_id("", key)


def fusion_to_phonk_params(
    prompt: str,
    variation: int,
    mode: FactoryModeProfile,
    *,
    lane: object | None = None,
    seed_override: int | None = None,
) -> PhonkParams:
    base = resolve_phonk_params(prompt, variation, lane)
    f = mode.fusion.clamped()
    grit = min(0.95, base.grit + f.drive * 0.45 + f.memphis_phonk_blend * 0.12)
    kick_pitch = base.kick_pitch - f.low_frequency_weight * 6.0
    kick_decay = max(0.12, base.kick_decay * (1.0 + (1 - f.transient_aggression) * 0.25))
    snare_snap = min(0.88, base.snare_snap * (1.0 + f.transient_aggression * 0.35))
    hat_bright = min(0.9, base.hat_bright * (1.0 + f.metallic_character * 0.2))
    cowbell = base.cowbell_tone * (1.0 + f.metallic_character * 0.18)
    seed = seed_override if seed_override is not None else base.seed
    return replace(
        base,
        kick_pitch=kick_pitch,
        kick_decay=kick_decay,
        snare_snap=snare_snap,
        hat_bright=hat_bright,
        cowbell_tone=cowbell,
        grit=grit,
        seed=seed,
    )


def apply_fusion_post(samples: list[float], fusion: FusionMacros) -> list[float]:
    f = fusion.clamped()
    hold = max(1, int(1 + f.vintage_character * 6))
    bits = max(4, 16 - int(f.vintage_character * 10))
    out = zero_order_hold(samples, hold)
    out = bit_depth_reduce(out, bits)
    k = f.drive * 4.0
    out = apply_saturation(out, k)
    fc = 30.0 + f.low_frequency_weight * 50.0
    out = low_shelf_boost(out, f.low_frequency_weight * 0.5, fc_hz=fc)
    return out


def multi_generation_dub(samples: list[float], generations: int = 3) -> list[float]:
    """Simulate repeated 4-track tape bounces (darken + soften each pass)."""
    from drum_dsp import one_pole_lp, zero_order_hold

    out = list(samples)
    for g in range(max(1, generations)):
        out = zero_order_hold(out, 2 + g)
        out = one_pole_lp(out, fc_hz=6800.0 - g * 900.0)
        gain = 0.92 - g * 0.04
        out = [s * gain for s in out]
    return out


def tape_dust_hiss(samples: list[float], amount: float, seed: int) -> list[float]:
    import math

    if amount <= 0:
        return list(samples)
    out: list[float] = []
    for i, s in enumerate(samples):
        t = i / 44100.0
        hiss = math.sin(t * 13007 + seed) * math.sin(t * 9127 + seed * 0.7)
        dust = 1.0 if ((i + seed) % 9971) > 9945 else 0.22
        out.append(s + hiss * amount * 0.018 + (dust - 1.0) * amount * 0.004)
    return out


def vinyl_crackle(samples: list[float], amount: float, seed: int) -> list[float]:
    import random

    rng = random.Random(seed)
    if amount <= 0:
        return list(samples)
    out = list(samples)
    for _ in range(int(len(samples) * amount * 0.0004)):
        idx = rng.randrange(0, max(1, len(out) - 1))
        out[idx] += rng.uniform(-0.09, 0.09) * amount
    return out


def occult_tape_bus(samples: list[float], fusion: FusionMacros, *, seed: int = 0) -> list[float]:
    """Worn 4-track dub, aggressive clip shave, wow/flutter tape sat, vinyl crackle."""
    import math

    from drum_dsp import hard_clip, low_shelf_boost, one_pole_lp, sigmoid_saturation

    f = fusion.clamped()
    out = multi_generation_dub(samples, generations=4)
    out = low_shelf_boost(out, 0.55 + f.low_frequency_weight * 0.3, fc_hz=48.0)
    out = [sigmoid_saturation(s, 3.5 + f.drive * 5) for s in out]
    clip_thr = 0.62 + (1.0 - f.drive) * 0.08
    out = hard_clip(out, clip_thr)
    sr = 44100
    wow = 0.006 + f.vintage_character * 0.012
    flutter = 0.003 + f.vintage_character * 0.007
    mod: list[float] = []
    for i, s in enumerate(out):
        t = i / sr
        w = 1.0 + wow * math.sin(t * 2.1 + seed * 0.002) + flutter * math.sin(t * 7.9 + i * 0.00001)
        sat = sigmoid_saturation(s * w, 2.0 + f.drive * 3.5)
        mod.append(sat)
    out = one_pole_lp(mod, fc_hz=4800.0)
    out = tape_dust_hiss(out, 0.35 + f.vintage_character * 0.55, seed)
    out = vinyl_crackle(out, 0.25 + f.vintage_character * 0.35, seed + 3)
    return hard_clip(out, clip_thr)


def black_cassette_bus(samples: list[float], fusion: FusionMacros) -> list[float]:
    """Drum-bus: hard clip → tape sat → 9 kHz LP (synthesized chain)."""
    from drum_dsp import hard_clip, one_pole_lp, sigmoid_saturation

    f = fusion.clamped()
    threshold = 0.82 - f.drive * 0.08
    out = hard_clip(samples, threshold)
    out = [sigmoid_saturation(s, 2.5 + f.drive * 6) for s in out]
    import math

    sr = 44100
    flutter = 0.003 + f.vintage_character * 0.008
    mod: list[float] = []
    for i, s in enumerate(out):
        t = i / sr
        mod.append(s * (1.0 + flutter * math.sin(t * 4.7 + i * 0.00002)))
    return one_pole_lp(mod, fc_hz=9000.0)
