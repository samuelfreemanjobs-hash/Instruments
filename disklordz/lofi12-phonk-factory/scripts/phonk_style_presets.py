"""Curated Memphis / Southern presets — Juicy J, DJ Paul, DJ Toomp."""

from __future__ import annotations

from dataclasses import dataclass

from phonk_loop_fx import LoopFxParams


@dataclass(frozen=True)
class StylePreset:
    preset_id: str
    label: str
    prompt: str
    lane: str | None
    engine: str | None
    bpm: float | None
    fx: LoopFxParams


PRESETS: dict[str, StylePreset] = {
    "juicy_j": StylePreset(
        "juicy_j",
        "Juicy J — dirty Memphis 808 + tape",
        "juicy j dirty memphis phonk 808 cowbell 86 bpm",
        "juicy_j",
        "juicy_j",
        86.0,
        LoopFxParams(filter_cutoff=0.52, reverb_send=0.24, tape=0.3, drive=0.3),
    ),
    "dj_paul": StylePreset(
        "dj_paul",
        "DJ Paul — bounce kicks, clap snare, cowbell",
        "dj paul three 6 memphis 808 cowbell dirty 84 bpm",
        "dj_paul",
        "juicy_j",
        84.0,
        LoopFxParams(filter_cutoff=0.48, reverb_send=0.22, tape=0.28, drive=0.32),
    ),
    "dj_toomp": StylePreset(
        "dj_toomp",
        "DJ Toomp — SP-1200 crunch + hard trap 808",
        "dj toomp atlanta memphis trap dirty 808 78 bpm",
        "dj_toomp",
        "dj_toomp",
        78.0,
        LoopFxParams(filter_cutoff=0.46, reverb_send=0.18, tape=0.26, drive=0.34),
    ),
    "memphis_trinity": StylePreset(
        "memphis_trinity",
        "Juicy J + DJ Paul + Toomp (default stack)",
        "juicy j dj paul toomp dirty memphis sp1200 86 bpm",
        "juicy_j",
        "juicy_j",
        86.0,
        LoopFxParams(filter_cutoff=0.5, reverb_send=0.22, tape=0.29, drive=0.31),
    ),
}


def list_preset_ids() -> list[str]:
    return list(PRESETS.keys())


def get_preset(preset_id: str) -> StylePreset | None:
    key = preset_id.strip().lower().replace("-", "_").replace(" ", "_")
    return PRESETS.get(key)
