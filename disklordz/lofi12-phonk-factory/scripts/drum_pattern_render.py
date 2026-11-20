"""Render audio loops from pattern notes (loose timing)."""

from __future__ import annotations

from drum_pattern_intel import GM, PatternNote
from phonk_synth import SRC_RATE, render_slot, resolve_phonk_params

SLOT_MAP = {
    "kick": "kick_808",
    "snare": "snare_memphis",
    "clap": "clap",
    "hat": "hat_closed",
    "hat_open": "hat_open",
    "cowbell": "cowbell_low",
    "rim": "snare_rim",
    "tom": "tom_tune_lo",
    "crash": "crash_cymbal",
}


ATL_SLOT_MAP = {
    "kick": "kick_atl_round",
    "snare": "snare_atl_bridged",
    "clap": "clap_atl_grit",
    "hat": "hat_atl_closed",
    "hat_open": "hat_atl_open",
    "tom": "tom_descend",
    "crash": "crash_cymbal",
    "rim": "fm_rim",
}


def _mix_at(buf: list[float], offset: int, sample: list[float], gain: float) -> None:
    for i, s in enumerate(sample):
        idx = offset + i
        if idx >= len(buf):
            break
        buf[idx] += s * gain


def render_pattern_loop(
    notes: list[PatternNote],
    *,
    bpm: float,
    bars: int,
    params,
    engine_id: str,
    cowbell_detune: float = 0.97,
    atl_trap: bool = False,
    trap_hard: bool = False,
) -> list[float]:
    beat = 60.0 / bpm
    step_sec = beat / 4.0
    length = int(bars * 4 * beat * SRC_RATE) + SRC_RATE // 2
    buf = [0.0] * length
    vel_scale = lambda v: v / 127.0

    for n in notes:
        t = n.step * step_sec + n.micro_ms / 1000.0
        offset = int(t * SRC_RATE)
        slot_map = ATL_SLOT_MAP if atl_trap else SLOT_MAP
        slot = slot_map.get(n.instrument, "perc_snap")
        if atl_trap and n.instrument == "kick" and trap_hard:
            slot = "kick_atl_hard"
        if n.instrument == "cowbell":
            from dataclasses import replace

            p = replace(params, cowbell_tone=params.cowbell_tone * cowbell_detune)
            hit = render_slot(slot, p, engine_id)
        elif n.instrument == "snare":
            body_slot = slot_map["snare"] if atl_trap else "snare_memphis"
            body = render_slot(body_slot, params, engine_id)
            if not atl_trap and n.velocity > 100:
                rim = render_slot("snare_rim", params, engine_id)
                nlen = max(len(body), len(rim))
                body = [
                    (body[i] if i < len(body) else 0.0) + (rim[i] if i < len(rim) else 0.0) * 0.55
                    for i in range(nlen)
                ]
            hit = body
        elif n.instrument == "kick" and not atl_trap:
            from phonk_synth import kick_sub, kick_woody_box

            wood = kick_woody_box(params)
            sub = kick_sub(params)
            nlen = max(len(wood), len(sub))
            hit = [(wood[i] if i < len(wood) else 0.0) + (sub[i] if i < len(sub) else 0.0) * 0.85 for i in range(nlen)]
        else:
            hit = render_slot(slot, params, engine_id)
        _mix_at(buf, offset, hit, vel_scale(n.velocity))

    return buf
