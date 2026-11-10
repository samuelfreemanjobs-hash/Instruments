"""Curated Memphis / Southern presets — Juicy J, DJ Paul, DJ Toomp."""

from __future__ import annotations

from dataclasses import dataclass

from phonk_loop_fx import LoopFxParams, fx_dict


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
        "Hard Dirt 808",
        "juicy j dirty memphis phonk 808 cowbell 86 bpm",
        "juicy_j",
        "juicy_j",
        86.0,
        LoopFxParams(
            filter_cutoff=0.52,
            reverb_send=0.24,
            tape=0.3,
            drive=0.3,
            cassette=0.15,
            bitcrush=0.22,
        ),
    ),
    "dj_paul": StylePreset(
        "dj_paul",
        "Bounce & Cowbell",
        "dj paul three 6 memphis 808 cowbell dirty 84 bpm",
        "dj_paul",
        "juicy_j",
        84.0,
        LoopFxParams(filter_cutoff=0.48, reverb_send=0.22, tape=0.28, drive=0.32),
    ),
    "dj_toomp": StylePreset(
        "dj_toomp",
        "Traproom Tape",
        "dj toomp atlanta memphis trap dirty 808 78 bpm",
        "dj_toomp",
        "dj_toomp",
        78.0,
        LoopFxParams(filter_cutoff=0.46, reverb_send=0.18, tape=0.26, drive=0.34),
    ),
    "memphis_trinity": StylePreset(
        "memphis_trinity",
        "Memphis Insanity",
        "dirty memphis phonk sp1200 808 cowbell 86 bpm",
        "juicy_j",
        "juicy_j",
        86.0,
        LoopFxParams(
            filter_cutoff=0.5,
            reverb_send=0.22,
            tape=0.29,
            drive=0.31,
            cassette=0.18,
            bitcrush=0.28,
        ),
    ),
}


def list_preset_ids() -> list[str]:
    return list(PRESETS.keys())


def get_preset(preset_id: str) -> StylePreset | None:
    key = preset_id.strip().lower().replace("-", "_").replace(" ", "_")
    return PRESETS.get(key)


def preset_to_api_dict(ps: StylePreset) -> dict:
    return {
        "id": ps.preset_id,
        "label": ps.label,
        "prompt": ps.prompt,
        "lane": ps.lane,
        "engine": ps.engine,
        "bpm": ps.bpm,
        "fx": fx_dict(ps.fx),
    }


def all_presets_api() -> list[dict]:
    return [preset_to_api_dict(ps) for ps in PRESETS.values()]


def merge_render_payload(payload: dict) -> dict:
    """Apply preset fields when ``preset`` id is set; explicit payload keys win."""
    preset_id = payload.get("preset")
    if not preset_id:
        return payload
    ps = get_preset(str(preset_id))
    if not ps:
        return payload
    out = dict(payload)
    out.setdefault("prompt", ps.prompt)
    if ps.lane and "lane" not in payload:
        out["lane"] = ps.lane
    if ps.engine and "engine" not in payload:
        out["engine"] = ps.engine
    if ps.bpm is not None and "bpm" not in payload:
        out["bpm"] = ps.bpm
    if ps.fx and not payload.get("fx"):
        out["fx"] = fx_dict(ps.fx)
    return out
