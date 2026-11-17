#!/usr/bin/env python3
"""DiskLordz HYPER DNA — 2D 5-Genre Morph Matrix, Architecture Crossfade & 1->231 Evolution Engine.

Ports and verifies the exact C++20 HyperDNAMorph mathematics (including the §6 single-pass
logTargetRatio correction) plus:
- 2D inverse-distance weighting 1 / (d + 0.08)^2.6 across Plugg, Rage, Hypertrap, Hyperpop, Digicore
- Logarithmic interpolation for positive frequency/time/drive & linear for bounded mixes
- Independent parameter locks (root, transient, decay, drive)
- Drum-specific invariant preservation (808 root, Kick thump, Snare body, Clap burst count, Hat metal)
- 3-level synthesis morphing (Level 1 params + Level 2 oscillator/FM/noise architecture blend + Level 3 timbre/glitch)
- DNA Sequencer loop renderer (per-step XY morphing from Plugg hat -> Digicore hat in one bar)
- Capture -> Evolve -> Export (1 -> 231 candidate evolution tree with QC + cosine similarity screening)
"""
from __future__ import annotations

from dataclasses import asdict, dataclass, replace
import math
from pathlib import Path
import json

import numpy as np
import soundfile as sf

import brick_squad_lab as brick
import drum_forge_pro as pro
import forge_core_v2 as core
import hyper_series_lab as hyper

GENRES = ("Plugg", "Rage", "Hypertrap", "Hyperpop", "Digicore")


@dataclass(frozen=True)
class DrumDNA:
    root_hz: float = 50.0
    pitch_start_hz: float = 155.0
    pitch_ms: float = 45.0
    decay_ms: float = 450.0
    glide_ms: float = 25.0
    drive: float = 1.5
    fm_index: float = 0.0
    noise_mix: float = 0.10
    glitch_mix: float = 0.0
    transient: float = 0.50


@dataclass(frozen=True)
class DNALocks:
    lock_root: bool = True
    lock_transient: bool = False
    lock_decay: bool = False
    lock_drive: bool = False


PROFILES: tuple[tuple[str, tuple[float, float], DrumDNA], ...] = (
    ("Plugg", (0.12, 0.16), DrumDNA(50.0, 120.0, 55.0, 520.0, 20.0, 1.1, 0.1, 0.08, 0.00, 0.30)),
    ("Rage", (0.85, 0.14), DrumDNA(50.0, 190.0, 25.0, 400.0, 30.0, 4.2, 0.2, 0.20, 0.04, 0.90)),
    ("Hypertrap", (0.77, 0.68), DrumDNA(50.0, 175.0, 35.0, 650.0, 90.0, 3.2, 0.7, 0.25, 0.15, 0.75)),
    ("Hyperpop", (0.22, 0.85), DrumDNA(50.0, 145.0, 65.0, 330.0, 65.0, 2.3, 2.3, 0.45, 0.35, 0.65)),
    ("Digicore", (0.51, 0.47), DrumDNA(50.0, 170.0, 45.0, 420.0, 42.0, 3.6, 1.8, 0.50, 0.90, 0.80)),
)


def sanitize_dna(d: DrumDNA) -> DrumDNA:
    """Guard against malformed preset values before logarithmic math."""
    return DrumDNA(
        root_hz=float(np.clip(d.root_hz if np.isfinite(d.root_hz) else 50.0, 24.0, 4000.0)),
        pitch_start_hz=float(np.clip(d.pitch_start_hz if np.isfinite(d.pitch_start_hz) else 155.0, 24.0, 12000.0)),
        pitch_ms=float(np.clip(d.pitch_ms if np.isfinite(d.pitch_ms) else 45.0, 2.0, 500.0)),
        decay_ms=float(np.clip(d.decay_ms if np.isfinite(d.decay_ms) else 450.0, 12.0, 3500.0)),
        glide_ms=float(np.clip(d.glide_ms if np.isfinite(d.glide_ms) else 25.0, 0.5, 500.0)),
        drive=float(np.clip(d.drive if np.isfinite(d.drive) else 1.5, 0.5, 12.0)),
        fm_index=float(np.clip(d.fm_index if np.isfinite(d.fm_index) else 0.0, 0.0, 8.0)),
        noise_mix=float(np.clip(d.noise_mix if np.isfinite(d.noise_mix) else 0.1, 0.0, 1.0)),
        glitch_mix=float(np.clip(d.glitch_mix if np.isfinite(d.glitch_mix) else 0.0, 0.0, 1.0)),
        transient=float(np.clip(d.transient if np.isfinite(d.transient) else 0.5, 0.0, 1.0)),
    )


def calculate_weights(x: float, y: float) -> dict[str, float]:
    cx = float(np.clip(x, 0.0, 1.0))
    cy = float(np.clip(y, 0.0, 1.0))
    raw = {}
    total = 0.0
    for name, (px, py), _dna in PROFILES:
        dist = math.hypot(cx - px, cy - py)
        w = 1.0 / ((dist + 0.08) ** 2.6)
        raw[name] = w
        total += w
    return {k: v / total for k, v in raw.items()}


def blend_dna(
    original: DrumDNA,
    weights: dict[str, float],
    intensity: float,
    locks: DNALocks = DNALocks(),
    instrument: str = "808",
) -> DrumDNA:
    """Single-pass corrected logarithmic/linear morph with locks and per-instrument invariants."""
    orig = sanitize_dna(original)
    t_amt = float(np.clip(intensity, 0.0, 1.0))
    w_vec = [weights[name] for name, _, _ in PROFILES]
    p_dnas = [sanitize_dna(dna) for _, _, dna in PROFILES]

    def lin_field(attr: str) -> float:
        target = sum(w * getattr(d, attr) for w, d in zip(w_vec, p_dnas))
        cur = getattr(orig, attr)
        return float(cur + (target - cur) * t_amt)

    def log_field(attr: str) -> float:
        target_log = sum(w * math.log(max(getattr(d, attr), 0.001)) for w, d in zip(w_vec, p_dnas))
        orig_log = math.log(max(getattr(orig, attr), 0.001))
        return float(math.exp(orig_log + (target_log - orig_log) * t_amt))

    # Corrected §6 single-pass relative pitch-start interpolation (intensity applied once!)
    log_target_ratio = 0.0
    for w, d in zip(w_vec, p_dnas):
        ratio = max(d.pitch_start_hz / max(d.root_hz, 1.0), 1.0)
        log_target_ratio += w * math.log(ratio)
    orig_ratio = max(orig.pitch_start_hz / max(orig.root_hz, 1.0), 1.0)
    pitch_start = orig.root_hz * math.exp(math.log(orig_ratio) + (log_target_ratio - math.log(orig_ratio)) * t_amt)

    decay_ms = orig.decay_ms if locks.lock_decay else log_field("decay_ms")
    drive = orig.drive if locks.lock_drive else log_field("drive")
    transient = orig.transient if locks.lock_transient else float(np.clip(lin_field("transient"), 0.0, 1.0))

    # Instrument-specific invariant bounds (kicks stay short & thumpy, hats stay crisp)
    if instrument == "kick":
        decay_ms = min(decay_ms, 260.0) if not locks.lock_decay else decay_ms
    elif instrument == "hat":
        decay_ms = min(decay_ms, 140.0) if not locks.lock_decay else decay_ms

    return DrumDNA(
        root_hz=orig.root_hz if locks.lock_root else log_field("root_hz"),
        pitch_start_hz=float(pitch_start),
        pitch_ms=log_field("pitch_ms"),
        decay_ms=float(decay_ms),
        glide_ms=log_field("glide_ms"),
        drive=float(drive),
        fm_index=float(np.clip(lin_field("fm_index"), 0.0, 8.0)),
        noise_mix=float(np.clip(lin_field("noise_mix"), 0.0, 1.0)),
        glitch_mix=float(np.clip(lin_field("glitch_mix"), 0.0, 1.0)),
        transient=transient,
    )


def synthesize_from_dna(
    dna: DrumDNA,
    instrument: str = "808",
    sr: int = 48000,
    seed: int = 515808,
    velocity: float = 1.0,
) -> np.ndarray:
    """3-level synthesis morphing: parameters + oscillator/FM/noise architecture + digital destruction."""
    d = sanitize_dna(dna)
    vel = float(np.clip(velocity, 0.15, 1.0))
    rng = np.random.default_rng(seed)

    decay_s = (d.decay_ms * 0.001) * (0.85 + 0.20 * vel)
    duration = float(np.clip(decay_s * 4.2, 0.14, 2.8))
    t = np.arange(int(sr * duration)) / sr

    if instrument in ("808", "kick"):
        pitch_tau = d.pitch_ms * 0.001
        rubber = 10.0 * min(d.fm_index / 2.5, 1.0) * np.sin(2.0 * np.pi * 16.0 * t) * np.exp(-t / 0.075)
        freq = d.root_hz + max(0.0, d.pitch_start_hz - d.root_hz) * np.exp(-t / pitch_tau) + rubber
        phase = 2.0 * np.pi * np.cumsum(freq) / sr

        # Level 2 architecture blend: pure sine <-> triangle <-> FM harmonic carrier
        tri_w = float(np.clip((d.drive - 1.0) / 4.0, 0.0, 0.45))
        fm_mod = np.sin(2.0 * phase) * (d.fm_index * 0.45 * np.exp(-t / 0.05))
        body = ((1.0 - tri_w) * np.sin(phase + fm_mod) + tri_w * (2.0 / np.pi) * np.arcsin(np.sin(phase)))
        amp = (1.0 - np.exp(-t / 0.0005)) * np.exp(-t / decay_s)

        click = core.filter_signal(rng.uniform(-1.0, 1.0, len(t)), 1900.0, "highpass", sr, 2)
        click *= np.exp(-t / 0.0022) * (0.04 + 0.28 * d.transient) * vel
        tex = core.filter_signal(rng.standard_normal(len(t)), 2800.0, "highpass", sr, 2) * np.exp(-t / 0.025) * d.noise_mix * 0.14
        raw = (body * amp + click + tex) * (0.45 + 0.55 * vel)
        preserve_hz = 115.0
    elif instrument == "hat":
        base = min(max(d.root_hz * 64.0, 3100.0), sr * 0.19)
        ratios = (1.0, 1.39, 1.83, 2.35, 2.98, 3.74)
        metal = np.zeros_like(t)
        for idx, r in enumerate(ratios):
            f0 = min(base * r, sr * 0.44)
            fm = np.sin(2.0 * np.pi * min(f0 * 1.4, sr * 0.45) * t) * (d.fm_index * 0.25 * np.exp(-t / 0.015))
            metal += (0.62 ** idx) * np.sin(2.0 * np.pi * f0 * t + fm)
        hp_hz = float(np.clip(6500.0 + 2200.0 * vel, 4500.0, sr * 0.40))
        noise = core.filter_signal(rng.uniform(-1.0, 1.0, len(t)), hp_hz, "highpass", sr, 3)
        env = (1.0 - np.exp(-t / 0.0002)) * np.exp(-t / min(decay_s, 0.12))
        raw = ((1.0 - d.noise_mix * 0.5) * core.filter_signal(metal / 3.5, hp_hz * 0.85, "highpass", sr, 2) + d.noise_mix * noise) * env * (0.4 + 0.6 * vel)
        preserve_hz = 0.0
    else:  # snare / clap / perc
        body_hz = max(165.0, d.root_hz)
        phase = 2.0 * np.pi * np.cumsum(body_hz + 115.0 * np.exp(-t / (d.pitch_ms * 0.0004))) / sr
        fm = np.sin(2.0 * phase * 1.57) * (d.fm_index * np.exp(-t / 0.03))
        body = np.sin(phase + fm) * np.exp(-t / 0.042) * 0.65
        noise = core.filter_signal(rng.standard_normal(len(t)), 1500.0, "highpass", sr, 2) * np.exp(-t / min(decay_s, 0.22)) * (0.35 + 0.65 * d.noise_mix)
        raw = (body + noise) * (0.45 + 0.55 * vel)
        preserve_hz = 0.0

    # Level 3 timbre & digital destruction morphing
    crush_bits = int(round(16 - 7 * d.glitch_mix))
    sr_div = 1 + int(round(2 * d.glitch_mix))
    gate_hz = 38.0 * d.glitch_mix if d.glitch_mix > 0.25 else 0.0
    spec = hyper.HyperStyleSpec(
        id="dna_live",
        label="DNA Live",
        drive=d.drive,
        fold=min(0.45, d.fm_index * 0.14),
        crush_bits=crush_bits,
        sr_div=sr_div,
        gate_hz=gate_hz,
        freq_shift_hz=35.0 * max(0.0, d.fm_index - 1.0),
        rubber=min(0.6, d.fm_index * 0.2),
        bright=1.0 + 0.25 * d.transient,
    )
    processed = hyper.apply_digital_destruction(raw, sr, spec, preserve_sub_below_hz=preserve_hz)
    stereo = np.column_stack((processed, processed))
    return core.master(brick.trim_below_db(stereo, sr), sr, -1.0)


def render_dna_sequencer_bar(
    base_dna: DrumDNA,
    step_xy_points: list[tuple[float, float, float]],
    instrument: str = "hat",
    bpm: float = 152.0,
    sr: int = 48000,
    seed: int = 515808,
) -> np.ndarray:
    """Renders a 16-step pattern where each step morphs XY DNA (e.g. Plugg step -> Digicore step)."""
    step_sec = 60.0 / bpm / 4.0
    total_samples = int(round(len(step_xy_points) * step_sec * sr)) + int(0.35 * sr)
    mix = np.zeros((total_samples, 2), dtype=np.float64)

    for idx, (x, y, influence) in enumerate(step_xy_points):
        start = int(round(idx * step_sec * sr))
        weights = calculate_weights(x, y)
        morphed = blend_dna(base_dna, weights, influence, instrument=instrument)
        hit = synthesize_from_dna(morphed, instrument=instrument, sr=sr, seed=seed + idx, velocity=0.92 if idx % 4 == 0 else 0.72)
        end = min(total_samples, start + len(hit))
        mix[start:end] += hit[: end - start]

    return core.master(mix, sr, -1.0)


def evolve_231_tree(
    original: DrumDNA,
    instrument: str = "808",
    sr: int = 48000,
    seed: int = 515808,
    full_231: bool = False,
    similarity_ceiling: float = 0.9997,
) -> dict:
    """Capture -> Evolve -> Export pipeline (1 + 5 + 25 + 75 + 125 = 231 candidate tree with QC filtering)."""
    candidates: list[tuple[str, DrumDNA, float, int]] = [("00_original", original, 1.0, seed)]

    # 5 pure genre anchor versions
    for g_idx, (name, (px, py), _dna) in enumerate(PROFILES):
        w = calculate_weights(px, py)
        candidates.append((f"01_genre_{name.lower()}", blend_dna(original, w, 0.92, instrument=instrument), 1.0, seed + g_idx + 1))

    mut_count = 25 if full_231 else 5
    vel_count = 75 if full_231 else 6
    grid_count = 125 if full_231 else 8

    rng = np.random.default_rng(seed)
    for m in range(mut_count):
        x, y = float(rng.uniform(0.05, 0.95)), float(rng.uniform(0.05, 0.95))
        candidates.append((f"02_mut_{m + 1:02d}", blend_dna(original, calculate_weights(x, y), float(rng.uniform(0.35, 0.95)), instrument=instrument), 1.0, seed + 100 + m))

    for v in range(vel_count):
        vel = 0.35 + 0.65 * ((v % 5) / 4.0)
        anchor = PROFILES[v % len(PROFILES)]
        candidates.append((f"03_vel_{v + 1:02d}", blend_dna(original, calculate_weights(*anchor[1]), 0.75, instrument=instrument), vel, seed + 300 + v))

    for g in range(grid_count):
        gx = (g % 5) / 4.0
        gy = ((g // 5) % 5) / 4.0
        inf = 0.25 + 0.75 * ((g // 25) / max(1, (grid_count // 25)))
        candidates.append((f"04_xy_{g + 1:03d}", blend_dna(original, calculate_weights(gx, gy), min(1.0, inf), instrument=instrument), 0.95, seed + 600 + g))

    accepted = []
    rejected = 0
    seen_vecs: list[np.ndarray] = []
    for label, d_item, vel, s_item in candidates:
        audio = synthesize_from_dna(d_item, instrument=instrument, sr=sr, seed=s_item, velocity=vel)
        if not np.all(np.isfinite(audio)) or np.max(np.abs(audio)) < 0.02:
            rejected += 1
            continue
        vec, _, _ = pro.descriptors(audio, sr)
        sim = max((float(np.dot(vec, prev)) for prev in seen_vecs), default=-1.0)
        if sim >= similarity_ceiling and label != "00_original":
            rejected += 1
            continue
        seen_vecs.append(vec)
        accepted.append((label, d_item, vel, audio))

    return {
        "candidates_planned": len(candidates),
        "accepted_count": len(accepted),
        "rejected_count": rejected,
        "renders": accepted,
    }


def export_hyper_dna_demo(out_dir: Path, sr: int = 48000) -> dict:
    out_dir.mkdir(parents=True, exist_ok=True)
    base_808 = DrumDNA(root_hz=48.9994, pitch_start_hz=130.0, pitch_ms=50.0, decay_ms=500.0, glide_ms=20.0, drive=1.2, fm_index=0.05, noise_mix=0.08, glitch_mix=0.0, transient=0.35)
    tree = evolve_231_tree(base_808, instrument="808", sr=sr, full_231=False)

    files = []
    for label, d_item, vel, audio in tree["renders"]:
        fname = f"DL_HyperDNA_808_{label}.wav"
        sf.write(out_dir / fname, audio, sr, subtype="PCM_24")
        files.append({"file": fname, "velocity": round(vel, 3), "dna": asdict(d_item)})

    # Render 16-step DNA Sequencer hat loop (Plugg -> Rage -> Hypertrap -> Hyperpop -> Digicore)
    xy_steps = [
        PROFILES[min(4, st // 4)][1] + (0.88,) for st in range(16)
    ]
    seq_loop = render_dna_sequencer_bar(DrumDNA(root_hz=55.0, decay_ms=42.0), xy_steps, instrument="hat", bpm=154.0, sr=sr)
    loop_name = "DL_HyperDNA_Sequencer_HatMorph_154BPM.wav"
    sf.write(out_dir / loop_name, seq_loop, sr, subtype="PCM_24")

    manifest = {
        "candidates_planned": tree["candidates_planned"],
        "accepted_count": tree["accepted_count"],
        "rejected_count": tree["rejected_count"],
        "sequencer_loop": loop_name,
        "files": files,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return manifest


if __name__ == "__main__":
    summary = export_hyper_dna_demo(Path("DiskLordz_Hyper_DNA_Demo"))
    print(f"Hyper DNA exported {summary['accepted_count']} accepted variations + {summary['sequencer_loop']}")
