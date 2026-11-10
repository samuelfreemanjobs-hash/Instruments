"""Nine factory sound modes + Black Cassette variant (Phase 1)."""

from __future__ import annotations

from dataclasses import dataclass

from drum_recipe import FusionMacros

FACTORY_MODE_IDS = (
    # Memphis
    "memphis_underground",
    "memphis_dark",
    "memphis_raw_southern",
    "memphis_vintage_drums",
    "memphis_dirty_cassette",
    # Phonk
    "phonk_og",
    "phonk_rare",
    "phonk_drift",
    "phonk_trap",
    "black_cassette",
    "occult_tape",
)

# Backward-compatible preset id → factory mode
PRESET_ALIASES: dict[str, str] = {
    "memphis_trinity": "memphis_underground",
    "juicy_j": "memphis_raw_southern",
    "dj_paul": "memphis_underground",
    "dj_toomp": "phonk_trap",
    "hard_dirt_808": "memphis_raw_southern",
    "bounce_cowbell": "memphis_underground",
    "traproom_tape": "phonk_trap",
    "memphis_insanity_pack": "memphis_underground",
}


def resolve_factory_mode(mode_or_preset: str | None) -> str:
    if not mode_or_preset:
        return "memphis_underground"
    key = mode_or_preset.strip().lower().replace("-", "_").replace(" ", "_")
    if key in FACTORY_MODE_IDS:
        return key
    return PRESET_ALIASES.get(key, "memphis_underground")


@dataclass(frozen=True)
class FactoryModeProfile:
    mode_id: str
    title: str
    engine: str
    lane: str | None
    bpm_lo: float
    bpm_hi: float
    swing_lo: float
    swing_hi: float
    fusion: FusionMacros
    prompt: str
    loop_bars: int = 4
    phonk_pattern: bool = False  # True = cowbell lines, fast hats


def _fm(**kwargs: float) -> FusionMacros:
    return FusionMacros(**kwargs)


MODES: dict[str, FactoryModeProfile] = {
    "memphis_underground": FactoryModeProfile(
        "memphis_underground",
        "Underground Memphis",
        "hard_dirt_808",
        "bounce_cowbell",
        110.0,
        130.0,
        0.54,
        0.58,
        _fm(
            memphis_phonk_blend=0.12,
            drive=0.42,
            vintage_character=0.45,
            transient_aggression=0.38,
            low_frequency_weight=0.55,
            metallic_character=0.35,
            density_swing=0.52,
        ),
        "underground memphis dry cassette 808 cowbell 118 bpm",
    ),
    "memphis_dark": FactoryModeProfile(
        "memphis_dark",
        "Dark Memphis",
        "mr_tape",
        "bounce_cowbell",
        112.0,
        128.0,
        0.54,
        0.57,
        _fm(
            memphis_phonk_blend=0.08,
            drive=0.35,
            vintage_character=0.55,
            transient_aggression=0.32,
            low_frequency_weight=0.62,
            metallic_character=0.28,
            density_swing=0.48,
        ),
        "dark memphis lowpass tape minor bell 120 bpm",
    ),
    "memphis_raw_southern": FactoryModeProfile(
        "memphis_raw_southern",
        "Raw Southern",
        "hard_dirt_808",
        "hard_dirt_808",
        118.0,
        130.0,
        0.52,
        0.56,
        _fm(
            memphis_phonk_blend=0.15,
            drive=0.48,
            vintage_character=0.22,
            transient_aggression=0.62,
            low_frequency_weight=0.45,
            metallic_character=0.42,
            density_swing=0.55,
        ),
        "raw southern snappy snare bright hat memphis 124 bpm",
    ),
    "memphis_vintage_drums": FactoryModeProfile(
        "memphis_vintage_drums",
        "Vintage Drum Machine",
        "tr808",
        None,
        110.0,
        125.0,
        0.52,
        0.55,
        _fm(
            memphis_phonk_blend=0.18,
            drive=0.28,
            vintage_character=0.35,
            transient_aggression=0.45,
            low_frequency_weight=0.4,
            metallic_character=0.55,
            density_swing=0.5,
        ),
        "vintage drum machine 808 909 quantized memphis 116 bpm",
    ),
    "memphis_dirty_cassette": FactoryModeProfile(
        "memphis_dirty_cassette",
        "Dirty Cassette",
        "mr_tape",
        "tape_lag",
        110.0,
        125.0,
        0.53,
        0.56,
        _fm(
            memphis_phonk_blend=0.1,
            drive=0.52,
            vintage_character=0.72,
            transient_aggression=0.4,
            low_frequency_weight=0.5,
            metallic_character=0.3,
            density_swing=0.46,
        ),
        "dirty cassette wow flutter memphis behind the beat 118 bpm",
    ),
    "phonk_og": FactoryModeProfile(
        "phonk_og",
        "OG Phonk",
        "hard_dirt_808",
        "hard_dirt_808",
        130.0,
        148.0,
        0.52,
        0.55,
        _fm(
            memphis_phonk_blend=0.72,
            drive=0.55,
            vintage_character=0.55,
            transient_aggression=0.48,
            low_frequency_weight=0.58,
            metallic_character=0.45,
            density_swing=0.58,
        ),
        "og phonk bitcrush tape bandpass 140 bpm",
        phonk_pattern=True,
    ),
    "phonk_rare": FactoryModeProfile(
        "phonk_rare",
        "Rare Phonk",
        "mr_tape",
        None,
        132.0,
        155.0,
        0.51,
        0.54,
        _fm(
            memphis_phonk_blend=0.78,
            drive=0.38,
            vintage_character=0.48,
            transient_aggression=0.35,
            low_frequency_weight=0.42,
            metallic_character=0.5,
            density_swing=0.52,
        ),
        "rare phonk ambient lo-fi flutter soft clip 142 bpm",
        phonk_pattern=True,
    ),
    "phonk_drift": FactoryModeProfile(
        "phonk_drift",
        "Drift Phonk",
        "tr909",
        None,
        140.0,
        165.0,
        0.5,
        0.53,
        _fm(
            memphis_phonk_blend=0.92,
            drive=0.78,
            vintage_character=0.25,
            transient_aggression=0.85,
            low_frequency_weight=0.7,
            metallic_character=0.62,
            density_swing=0.72,
        ),
        "drift phonk distorted 808 hard clip 155 bpm",
        phonk_pattern=True,
    ),
    "phonk_trap": FactoryModeProfile(
        "phonk_trap",
        "Phonk Trap",
        "traproom_tape",
        "traproom_tape",
        135.0,
        160.0,
        0.51,
        0.54,
        _fm(
            memphis_phonk_blend=0.85,
            drive=0.62,
            vintage_character=0.3,
            transient_aggression=0.72,
            low_frequency_weight=0.55,
            metallic_character=0.58,
            density_swing=0.68,
        ),
        "phonk trap transient polish compressed hats 150 bpm",
        phonk_pattern=True,
    ),
    "occult_tape": FactoryModeProfile(
        "occult_tape",
        "Occult Tape",
        "mr_tape",
        "tape_lag",
        110.0,
        120.0,
        0.52,
        0.56,
        _fm(
            memphis_phonk_blend=0.04,
            drive=0.84,
            vintage_character=0.9,
            transient_aggression=0.62,
            low_frequency_weight=0.7,
            metallic_character=0.38,
            density_swing=0.38,
        ),
        "occult tape slow dragging menacing worn 4-track 115 bpm",
        loop_bars=2,
    ),
    "black_cassette": FactoryModeProfile(
        "black_cassette",
        "Black Cassette",
        "hard_dirt_808",
        "tape_lag",
        130.0,
        140.0,
        0.52,
        0.56,
        _fm(
            memphis_phonk_blend=0.68,
            drive=0.82,
            vintage_character=0.68,
            transient_aggression=0.78,
            low_frequency_weight=0.72,
            metallic_character=0.55,
            density_swing=0.64,
        ),
        "black cassette dark underground phonk memphis tape 135 bpm",
        phonk_pattern=True,
    ),
}


def get_mode(mode_id: str) -> FactoryModeProfile:
    key = resolve_factory_mode(mode_id)
    return MODES[key]
