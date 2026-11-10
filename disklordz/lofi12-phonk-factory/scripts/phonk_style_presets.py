"""Curated factory presets (neutral naming; legacy ids alias to factory modes)."""

from __future__ import annotations

from dataclasses import dataclass

from factory_modes import MODES, PRESET_ALIASES, get_mode, resolve_factory_mode
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
    factory_mode: str | None = None


def _fx_from_mode(mode_id: str) -> LoopFxParams:
    m = get_mode(mode_id)
    f = m.fusion
    return LoopFxParams(
        filter_cutoff=0.45 + (1 - f.vintage_character) * 0.35,
        reverb_send=0.15 + f.metallic_character * 0.15,
        tape=f.vintage_character * 0.45,
        drive=f.drive * 0.5,
        cassette=f.vintage_character * 0.35,
        bitcrush=f.vintage_character * 0.4,
    )


def _style_from_mode(mode_id: str) -> StylePreset:
    m = get_mode(mode_id)
    return StylePreset(
        mode_id,
        m.title,
        m.prompt,
        m.lane,
        m.engine,
        (m.bpm_lo + m.bpm_hi) / 2.0,
        _fx_from_mode(mode_id),
        factory_mode=mode_id,
    )


PRESETS: dict[str, StylePreset] = {mid: _style_from_mode(mid) for mid in MODES}

PRESETS["memphis_insanity_pack"] = StylePreset(
    "memphis_insanity_pack",
    "Memphis Insanity Pack",
    PRESETS["memphis_underground"].prompt,
    "bounce_cowbell",
    "hard_dirt_808",
    118.0,
    _fx_from_mode("memphis_underground"),
    factory_mode="memphis_underground",
)

# Legacy aliases (backward compatible)
for old_id, mode_id in PRESET_ALIASES.items():
    if old_id in PRESETS:
        continue
    base = PRESETS[mode_id]
    PRESETS[old_id] = StylePreset(
        old_id,
        base.label,
        base.prompt,
        base.lane,
        base.engine,
        base.bpm,
        base.fx,
        factory_mode=mode_id,
    )


def list_preset_ids() -> list[str]:
    return list(PRESETS.keys())


def get_preset(preset_id: str) -> StylePreset | None:
    key = preset_id.strip().lower().replace("-", "_").replace(" ", "_")
    if key in PRESETS:
        return PRESETS[key]
    mode = resolve_factory_mode(key)
    if mode in PRESETS:
        return PRESETS[mode]
    return PRESETS.get(PRESET_ALIASES.get(key, ""))


def preset_to_api_dict(ps: StylePreset) -> dict:
    return {
        "id": ps.preset_id,
        "label": ps.label,
        "prompt": ps.prompt,
        "lane": ps.lane,
        "engine": ps.engine,
        "bpm": ps.bpm,
        "fx": fx_dict(ps.fx),
        "factoryMode": ps.factory_mode,
    }


def all_presets_api() -> list[dict]:
    return [preset_to_api_dict(ps) for ps in PRESETS.values()]


def merge_render_payload(payload: dict) -> dict:
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
    if ps.factory_mode and "factoryMode" not in payload:
        out["factoryMode"] = ps.factory_mode
    if ps.fx and not payload.get("fx"):
        out["fx"] = fx_dict(ps.fx)
    return out
