"""Render Memphis phonk loops from groove patterns + phonk_synth one-shots."""

from __future__ import annotations

from phonk_groove import (
    STEPS_PER_BAR,
    build_loop_pattern,
    effective_groove_bpm,
    validate_bpm,
)
from phonk_memphis_lanes import resolve_memphis_lane
from phonk_machine_engine import PROFILES, resolve_engine_id
from phonk_loop_fx import LoopFxParams, apply_loop_fx, fx_dict, fx_from_prompt
from phonk_synth import render_slot, resolve_phonk_params

SRC_RATE = 44100

SAMPLE_KEY: dict[str, str] = {
    "kick": "kick_808",
    "kick_dist": "kick_dist",
    "snare": "snare_memphis",
    "clap": "clap",
    "hat": "hat_closed",
    "hat_open": "hat_open",
    "cowbell": "cowbell_low",
    "rim": "snare_rim",
}


def _mix_at(buf: list[float], offset: int, sample: list[float], gain: float = 1.0) -> None:
    for i, s in enumerate(sample):
        idx = offset + i
        if idx >= len(buf):
            break
        buf[idx] += s * gain


def _duck_buffer(buf: list[float], offset: int, length: int, amount: float = 0.35) -> None:
    end = min(len(buf), offset + length)
    for i in range(offset, end):
        buf[i] *= 1.0 - amount


def render_phonk_loop(
    *,
    prompt: str,
    bpm: float,
    bars: int = 2,
    variation: int = 0,
    seed_override: int | None = None,
    lane_id: str | None = None,
    with_vocals: bool = False,
    vocal_gain: float = 0.32,
    engine_id: str | None = None,
    fx: LoopFxParams | None = None,
) -> tuple[list[float], dict]:
    bpm = validate_bpm(bpm)
    lane = resolve_memphis_lane(prompt, lane_id)
    engine = resolve_engine_id(prompt, engine_id)
    params = resolve_phonk_params(prompt, variation, lane)
    if seed_override is not None:
        params.seed = seed_override

    patterns = build_loop_pattern(
        bpm=bpm, bars=bars, seed=params.seed, variation=variation, lane=lane
    )
    step_samples = int((60.0 / bpm) / 4 * SRC_RATE)
    total_steps = bars * STEPS_PER_BAR
    n = step_samples * total_steps
    out = [0.0] * n

    sample_cache: dict[str, list[float]] = {}

    def get_sample(key: str) -> list[float]:
        if key not in sample_cache:
            sample_cache[key] = render_slot(SAMPLE_KEY[key], params, engine)
        return sample_cache[key]

    meta_hits = 0
    for bar_i, bar in enumerate(patterns):
        for hit in bar.hits:
            meta_hits += 1
            step_global = bar_i * STEPS_PER_BAR + hit.step
            offset = int(step_global * step_samples + hit.micro_delay * SRC_RATE)
            pcm = get_sample(hit.instrument)
            _mix_at(out, offset, pcm, hit.velocity)
            if hit.instrument in ("kick", "kick_dist"):
                _duck_buffer(out, offset, min(len(pcm), step_samples * 2), 0.25)

    if with_vocals:
        from phonk_vocal_synth import render_vocal_loop

        vox, vox_meta = render_vocal_loop(
            prompt=prompt, bpm=bpm, bars=bars, variation=variation, lane=lane
        )
        for i, s in enumerate(vox):
            if i < len(out):
                out[i] += s * vocal_gain

    fx_params = fx if fx is not None else fx_from_prompt(prompt)
    out = apply_loop_fx(out, fx_params, seed=params.seed)

    duration = n / SRC_RATE
    meta = {
        "bpm": bpm,
        "bars": bars,
        "durationSec": round(duration, 4),
        "sampleRate": SRC_RATE,
        "hitCount": meta_hits,
        "effectiveGrooveBpm": round(effective_groove_bpm(bpm), 2),
        "prompt": prompt,
        "variation": variation,
        "seed": params.seed,
        "memphisLane": lane.lane_id if lane else None,
        "memphisLaneName": lane.display_name if lane else None,
        "withVocals": with_vocals,
        "drumEngine": engine,
        "drumEngineLabel": PROFILES.get(engine, PROFILES["mr_tape"]).label,
        "fx": fx_dict(fx_params),
    }
    return out, meta


def bars_that_fit_lofi12(bpm: float, rate: int, max_sec: float) -> int:
    bar_sec = (60.0 / bpm) * 4
    return max(1, min(4, int(max_sec / bar_sec)))
