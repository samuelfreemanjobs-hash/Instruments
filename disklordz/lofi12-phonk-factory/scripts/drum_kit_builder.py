"""Full commercial kit builder (~150 one-shots + loops + MIDI)."""

from __future__ import annotations

import json
import math
import struct
import uuid
import wave
from dataclasses import dataclass
from pathlib import Path

from drum_fusion import (
    apply_fusion_post,
    black_cassette_bus,
    fusion_to_phonk_params,
    occult_tape_bus,
    resolve_engine,
)
from drum_pattern_intel import build_pattern
from drum_pattern_render import render_pattern_loop
from drum_qa import process_loop, process_one_shot
from drum_recipe import DrumRecipe, FusionMacros
from drum_texture_synth import synth_eerie_bell_motif, synth_minor_pad
from drum_variation import render_unique, recipe_variation
from factory_modes import FactoryModeProfile, get_mode
from phonk_loop_render import render_phonk_loop
from phonk_memphis_lanes import resolve_memphis_lane
from phonk_synth import SRC_RATE, kick_808, kick_sub, render_slot
from smf_export import write_pattern_smf

SUB_KEYS = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
KEY_LABELS = ("Am", "Dm", "Em", "Bm", "Fm", "Gm")


@dataclass(frozen=True)
class KitSizeSpec:
    name: str
    kicks: int
    snares: int
    claps: int
    hats_closed: int
    hats_open: int
    subs_808: int
    cowbells: int
    perc_fx: int
    drum_loops: int
    texture_loops: int = 0


KIT_SIZES: dict[str, KitSizeSpec] = {
    "demo": KitSizeSpec(
        "demo", 4, 4, 3, 4, 3, 3, 3, 4, 3, texture_loops=1
    ),
    "full": KitSizeSpec(
        "full", 18, 22, 12, 18, 11, 12, 12, 25, 18, texture_loops=4
    ),
    "lofi12": KitSizeSpec(
        "lofi12", 4, 4, 2, 2, 2, 2, 2, 2, 0, texture_loops=0
    ),
}


def _write_wav(path: Path, samples: list[float], sr: int = SRC_RATE) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sr)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767)) for s in samples
        )
        w.writeframes(frames)


def _mix(a: list[float], b: list[float]) -> list[float]:
    n = max(len(a), len(b))
    out = [0.0] * n
    for i in range(n):
        out[i] = (a[i] if i < len(a) else 0.0) + (b[i] if i < len(b) else 0.0)
    return out


def _render_voice(
    mode: FactoryModeProfile,
    slot: str,
    variation: int,
    *,
    pitch_mul: float = 1.0,
) -> tuple[list[float], DrumRecipe]:
    lane = resolve_memphis_lane(mode.prompt, mode.lane)
    params = fusion_to_phonk_params(mode.prompt, variation, mode, lane=lane, seed_override=variation * 9973)
    if pitch_mul != 1.0:
        from dataclasses import replace

        params = replace(
            params,
            kick_pitch=params.kick_pitch + 12 * math.log2(pitch_mul),
            cowbell_tone=params.cowbell_tone * pitch_mul,
        )
    engine = resolve_engine(mode.engine)
    raw = render_slot(slot, params, engine)
    fusion = mode.fusion
    if mode.mode_id == "black_cassette":
        raw = black_cassette_bus(apply_fusion_post(raw, fusion), fusion)
    elif mode.mode_id == "occult_tape":
        raw = occult_tape_bus(apply_fusion_post(raw, fusion), fusion, seed=variation)
    else:
        raw = apply_fusion_post(raw, fusion)
    if slot.startswith("kick"):
        from phonk_synth import kick_woody_box

        sub = kick_sub(params)
        if mode.mode_id == "occult_tape":
            layered = _mix(kick_woody_box(params), sub)
        else:
            layered = _mix(kick_808(params), sub)
        if mode.mode_id in ("black_cassette", "occult_tape"):
            from drum_dsp import hard_clip

            layered = hard_clip(layered, 0.72)
        raw = _mix(raw, layered[: len(raw)]) if len(raw) else layered
    cleaned, _ = process_one_shot(raw)
    recipe = DrumRecipe(
        recipe_id=str(uuid.uuid4()),
        factory_mode=mode.mode_id,
        voice_kind=slot,
        random_seed=variation,
        fusion=fusion,
        voice_params={"slot": slot, "pitch_mul": pitch_mul},
    )
    return cleaned, recipe


def _pitch_for_index(i: int, total: int) -> float:
    return 2 ** ((i % 12 - 6) / 12.0)


def build_kit(
    *,
    mode_id: str,
    out_dir: Path,
    kit_size: str = "full",
    seed: int = 0,
) -> dict:
    mode = get_mode(mode_id)
    spec = KIT_SIZES.get(kit_size, KIT_SIZES["full"])
    out_dir.mkdir(parents=True, exist_ok=True)
    seen: list[tuple[float, ...]] = []
    manifest_samples: list[dict] = []

    def emit(category: str, filename: str, slot: str, idx: int) -> None:
        pitch = _pitch_for_index(idx, 32)
        var = seed + idx * 131

        def render():
            pcm, _ = _render_voice(mode, slot, var, pitch_mul=pitch)
            return pcm

        pcm = render_unique(render, seen) or render()
        _, recipe = _render_voice(mode, slot, var, pitch_mul=pitch)
        rel = Path("One-Shots") / category / filename
        path = out_dir / rel
        _write_wav(path, pcm)
        manifest_samples.append(
            {
                "path": str(rel).replace("\\", "/"),
                "category": category,
                "recipe": recipe.to_dict(),
            }
        )

    for i in range(spec.kicks):
        emit("kicks", f"kick_{i+1:02d}.wav", "kick_808" if i % 3 else "kick_dist", i)
    for i in range(spec.snares):
        slot = "snare_memphis" if i % 4 else "snare_rim"
        emit("snares", f"snare_{i+1:02d}.wav", slot, 100 + i)
    for i in range(spec.claps):
        emit("claps", f"clap_{i+1:02d}.wav", "clap", 200 + i)
    for i in range(spec.hats_closed):
        emit("closed_hats", f"hat_closed_{i+1:02d}.wav", "hat_closed", 300 + i)
    for i in range(spec.hats_open):
        emit("open_hats", f"hat_open_{i+1:02d}.wav", "hat_open", 350 + i)
    for i in range(spec.subs_808):
        key = SUB_KEYS[i % len(SUB_KEYS)]
        emit("808s", f"sub808_{key}_{i+1:02d}.wav", "kick_sub", 400 + i)
    for i in range(spec.cowbells):
        key = SUB_KEYS[(i * 2) % len(SUB_KEYS)]
        slot = "cowbell_low" if i % 2 == 0 else "cowbell_high"
        emit("cowbells", f"cowbell_{key}_{i+1:02d}.wav", slot, 500 + i)
    for i in range(spec.perc_fx):
        slots = ("perc_snap", "tom_low", "tom_hi", "fx_impact", "fx_riser")
        emit("perc_fx", f"perc_{i+1:02d}.wav", slots[i % len(slots)], 600 + i)

    loops_meta: list[dict] = []
    for i in range(spec.drum_loops):
        notes, bpm, swing, key = build_pattern(mode, bars=mode.loop_bars, seed=seed + i * 17)
        lane = mode.lane
        engine = resolve_engine(mode.engine)
        if mode.mode_id == "occult_tape":
            p = fusion_to_phonk_params(
                mode.prompt, seed + i, mode, lane=resolve_memphis_lane(mode.prompt, lane), seed_override=seed + i
            )
            detune = 0.94 + ((seed + i) % 7) * 0.01
            pcm = render_pattern_loop(
                notes,
                bpm=bpm,
                bars=mode.loop_bars,
                params=p,
                engine_id=engine,
                cowbell_detune=detune,
            )
            pcm = occult_tape_bus(pcm, mode.fusion, seed=seed + i)
            pad = synth_minor_pad(bars=mode.loop_bars, bpm=bpm, key=KEY_LABELS[i % len(KEY_LABELS)], seed=seed + i)
            bell = synth_eerie_bell_motif(bpm=bpm, key=KEY_LABELS[i % len(KEY_LABELS)], seed=seed + i)
            mix_len = max(len(pcm), len(pad), len(bell))
            mixed = [0.0] * mix_len
            for src, g in ((pcm, 1.0), (pad, 0.42), (bell, 0.28)):
                for j, s in enumerate(src):
                    mixed[j] += s * g
            mixed = occult_tape_bus(mixed, mode.fusion, seed=seed + i + 99)
            pcm, _ = process_loop(mixed)
            meta = {"bpm": bpm, "swing": swing, "engine": engine, "pattern": "occult_tape"}
        else:
            pcm, meta = render_phonk_loop(
                prompt=mode.prompt,
                bpm=bpm,
                bars=mode.loop_bars,
                variation=i,
                seed_override=seed + i,
                lane_id=lane,
                engine_id=engine,
            )
            if mode.mode_id == "black_cassette":
                pcm = black_cassette_bus(pcm, mode.fusion)
            pcm, _ = process_loop(pcm)
        name = f"loop_{int(bpm)}bpm_{key}_{i+1:02d}.wav"
        rel = Path("Loops") / name
        _write_wav(out_dir / rel, pcm)
        midi_path = out_dir / "MIDI" / name.replace(".wav", ".mid")
        write_pattern_smf(midi_path, notes, bpm=bpm)
        loops_meta.append(
            {
                "wav": str(rel),
                "midi": str(Path("MIDI") / midi_path.name),
                "bpm": bpm,
                "key": key,
                "swing": swing,
                **meta,
            }
        )

    textures: list[dict] = []
    for i in range(spec.texture_loops):
        bpm = mode.bpm_lo + (i % 3) * 2
        k = KEY_LABELS[i % len(KEY_LABELS)]
        pad = synth_minor_pad(bars=2, bpm=bpm, key=k, seed=seed + 900 + i)
        bell = synth_eerie_bell_motif(bpm=bpm, key=k, seed=seed + 901 + i)
        for label, pcm in (("pad", pad), ("bell", bell)):
            pcm, _ = process_loop(pcm)
            fn = f"texture_{label}_{int(bpm)}bpm_{k}_{i+1:02d}.wav"
            rel = Path("Loops") / "textures" / fn
            _write_wav(out_dir / rel, pcm)
            textures.append({"path": str(rel), "type": label, "bpm": bpm, "key": k})

    manifest = {
        "format": "DISKLORDZ_DRUM_KIT",
        "version": 2,
        "factoryMode": mode.mode_id,
        "title": mode.title,
        "kitSize": spec.name,
        "seed": seed,
        "synthesis": "procedural_only",
        "sampleRate": SRC_RATE,
        "oneShotCount": len(manifest_samples),
        "loopCount": len(loops_meta),
        "samples": manifest_samples,
        "loops": loops_meta,
        "textureLoops": textures,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2))
    return manifest
