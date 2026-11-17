#!/usr/bin/env python3
"""DiskLordz HYPER SERIES — 20 Digital & Futuristic Drum Kit Styles + Hyper DNA Morph Engine.

Implements:
- Separate Hypertrap Engine (400-one-shot spec) vs Hyperpop Engine (400-one-shot spec)
- All 20 Hyper Series kit style banks configuring shared DSP modules (FM, RubberPitchEnvelope,
  MetallicResonator, BitCrusher/SR-Reducer, Wavefolder, GlitchGate, FrequencyShifter, OversampledDistortion)
- Five normative signature sounds:
  1. Hypertrap Nuclear 808 (48 Hz sub, 180->48 Hz @ 28 ms, 550 ms amp tau, parallel wavefolded saturation)
  2. Hyperpop Bubble Kick (110->62 Hz rubber pitch drop @ 45 ms, tri/sine morph, plastic FM click)
  3. Glitch Glass Snare (220 Hz body, 2.8 kHz band-passed noise, metallic partials, gated tail)
  4. Hyperplugg Crystal Hat (6 inharmonic oscillators, 9 kHz HPF, 32 ms decay tau)
  5. Digital Alien Perc (950 Hz FM carrier, 3.2x modulator ratio, falling FM index, 120 ms decay tau)
- Hyper DNA Morph Engine: takes one drum seed/root and morphs smoothly across
  Plugg (0.0) -> Rage (0.25) -> Hypertrap (0.50) -> Hyperpop (0.75) -> Digicore (1.0)
"""
from __future__ import annotations

import argparse
import json
import zipfile
from dataclasses import asdict, dataclass
from pathlib import Path

import numpy as np
import soundfile as sf
from scipy.signal import butter, hilbert, resample_poly, sosfilt

import brick_squad_lab as brick
import forge_core_v2 as core

ENGINE = "DiskLordz HYPER SERIES LAB v1"


@dataclass(frozen=True)
class HyperStyleSpec:
    id: str
    label: str
    drive: float
    fold: float
    crush_bits: int       # 16 = clean, 6..12 = crushed
    sr_div: int           # 1 = full SR, 2..6 = sample-rate decimation
    gate_hz: float        # 0 = off, >0 = rhythmic glitch gate
    freq_shift_hz: float  # single-sideband Hilbert frequency shift on upper metallic/texture layer
    rubber: float         # springy pitch bounce
    bright: float


HYPER_20_STYLES: dict[str, HyperStyleSpec] = {
    "hypertrap": HyperStyleSpec("hypertrap", "01 Hypertrap — Digital Warfare", 4.6, 0.32, 16, 1, 0.0, 0.0, 0.10, 1.22),
    "hyperpop": HyperStyleSpec("hyperpop", "02 Hyperpop — Plastic Dreams", 3.1, 0.28, 12, 2, 28.0, 45.0, 0.55, 1.38),
    "hyperplugg": HyperStyleSpec("hyperplugg", "03 Hyperplugg — Crystal Bounce", 1.7, 0.08, 16, 1, 0.0, 18.0, 0.18, 1.30),
    "digicore": HyperStyleSpec("digicore", "04 Digicore — Internet Drums", 3.8, 0.22, 11, 2, 34.0, 22.0, 0.25, 1.18),
    "glitchcore": HyperStyleSpec("glitchcore", "05 Glitchcore", 4.0, 0.35, 9, 3, 52.0, 85.0, 0.30, 1.28),
    "hexd": HyperStyleSpec("hexd", "06 HexD — Digital Decay", 5.4, 0.45, 8, 4, 24.0, -35.0, 0.12, 0.86),
    "sigilkore": HyperStyleSpec("sigilkore", "07 Sigilkore", 4.8, 0.38, 10, 3, 42.0, -65.0, 0.15, 0.94),
    "scenecore": HyperStyleSpec("scenecore", "08 Scenecore", 3.2, 0.20, 12, 2, 0.0, 60.0, 0.42, 1.42),
    "dariacore": HyperStyleSpec("dariacore", "09 Dariacore", 4.5, 0.40, 10, 2, 46.0, 110.0, 0.48, 1.35),
    "digital_jerk": HyperStyleSpec("digital_jerk", "10 Jerk / Digital Jerk", 2.3, 0.10, 14, 1, 0.0, 0.0, 0.32, 1.12),
    "new_jazz": HyperStyleSpec("new_jazz", "11 New Jazz", 2.1, 0.14, 15, 1, 0.0, 28.0, 0.36, 1.16),
    "dreamtrap": HyperStyleSpec("dreamtrap", "12 Dreamtrap", 1.5, 0.05, 16, 1, 0.0, 12.0, 0.14, 1.08),
    "cloud_rap": HyperStyleSpec("cloud_rap", "13 Cloud Rap", 1.6, 0.04, 15, 1, 0.0, 0.0, 0.08, 0.92),
    "bubblegum_bass": HyperStyleSpec("bubblegum_bass", "14 Bubblegum Bass", 2.6, 0.24, 14, 1, 0.0, 75.0, 0.72, 1.45),
    "deconstructed_club": HyperStyleSpec("deconstructed_club", "15 Deconstructed Club", 4.9, 0.48, 11, 2, 38.0, 140.0, 0.22, 1.32),
    "nightcore_trap": HyperStyleSpec("nightcore_trap", "16 Nightcore Trap", 3.4, 0.22, 13, 1, 0.0, 95.0, 0.38, 1.44),
    "cybertrap": HyperStyleSpec("cybertrap", "17 Cybertrap", 4.3, 0.36, 12, 2, 30.0, -48.0, 0.20, 1.24),
    "electropop_trap": HyperStyleSpec("electropop_trap", "18 Electropop Trap", 2.7, 0.15, 16, 1, 0.0, 20.0, 0.28, 1.26),
    "trance_trap": HyperStyleSpec("trance_trap", "19 Trance Trap", 2.9, 0.18, 16, 1, 0.0, 32.0, 0.24, 1.34),
    "experimental_digital_trap": HyperStyleSpec("experimental_digital_trap", "20 Experimental Digital Trap", 4.4, 0.50, 10, 3, 40.0, 125.0, 0.45, 1.30),
}

HYPERTRAP_400_SPEC = {
    "01_Distorted_Sliding_808s": 60,
    "02_Hyper_Kicks": 40,
    "03_Digital_Snares": 40,
    "04_Hard_Claps": 40,
    "05_Metallic_HiHats": 60,
    "06_Open_Hats_Cymbals": 30,
    "07_Robotic_Percussion": 40,
    "08_Lasers_FM_Hits": 40,
    "09_Glitch_Transients": 30,
    "10_Risers_Impacts": 20,
}

HYPERPOP_400_SPEC = {
    "01_Plastic_Rubber_Kicks": 40,
    "02_Distorted_Bass_Hits": 40,
    "03_Explosive_Snares": 50,
    "04_Synthetic_Claps": 50,
    "05_Digital_HiHats": 50,
    "06_Open_Hats_Cymbals": 30,
    "07_FM_Bubble_Percussion": 50,
    "08_Glitch_Bitcrushed_Hits": 40,
    "09_Synthetic_Impacts": 30,
    "10_Transitions_FX": 20,
}

DNA_ANCHORS: tuple[tuple[str, HyperStyleSpec], ...] = (
    ("plugg", HyperStyleSpec("plugg", "Plugg", 1.45, 0.03, 16, 1, 0.0, 0.0, 0.10, 1.15)),
    ("rage", HyperStyleSpec("rage", "Rage", 4.40, 0.24, 15, 1, 0.0, 0.0, 0.12, 1.18)),
    ("hypertrap", HYPER_20_STYLES["hypertrap"]),
    ("hyperpop", HYPER_20_STYLES["hyperpop"]),
    ("digicore", HYPER_20_STYLES["digicore"]),
)


def oversampled_wavefolder(x: np.ndarray, drive: float = 3.0, fold: float = 0.25) -> np.ndarray:
    up = resample_poly(x, 4, 1, axis=0)
    driven = up * max(drive, 0.5)
    asym = np.where(driven >= 0.0, np.tanh(driven * 1.15), np.tanh(driven * 0.9) * 0.94)
    if fold > 0.0:
        folded = np.sin(driven * (1.0 + 1.6 * fold))
        shaped = (1.0 - fold) * asym + fold * folded
    else:
        shaped = asym
    down = resample_poly(shaped, 1, 4, axis=0)[: len(x)]
    if len(down) < len(x):
        pad_shape = ((0, len(x) - len(down)), (0, 0)) if x.ndim == 2 else (0, len(x) - len(down))
        down = np.pad(down, pad_shape)
    return down - np.mean(down, axis=0)


def bitcrush_and_decimate(x: np.ndarray, bits: int = 16, sr_div: int = 1) -> np.ndarray:
    y = np.asarray(x, dtype=np.float64).copy()
    if sr_div > 1:
        idx = (np.arange(len(y)) // sr_div) * sr_div
        y = y[np.minimum(idx, len(y) - 1)]
    if bits < 16:
        levels = float(2 ** max(bits - 1, 2))
        y = np.round(y * levels) / levels
    return y


def frequency_shift_ssb(x: np.ndarray, sr: int, shift_hz: float) -> np.ndarray:
    if abs(shift_hz) < 1e-3:
        return x
    t = np.arange(len(x)) / sr
    analytic = hilbert(x, axis=0)
    carrier = np.exp(1j * 2.0 * np.pi * shift_hz * t)
    if x.ndim == 2:
        carrier = carrier[:, None]
    return np.real(analytic * carrier)


def glitch_gate(x: np.ndarray, sr: int, gate_hz: float, mix: float = 0.55) -> np.ndarray:
    if gate_hz <= 0.0:
        return x
    t = np.arange(len(x)) / sr
    # Preserve initial 32 ms transient punch, chop tail rhythmically
    square_gate = np.where(t < 0.032, 1.0, (np.sin(2.0 * np.pi * gate_hz * t) > -0.15).astype(np.float64))
    # Smooth gate edges slightly over 1.2 ms to prevent harsh DC pops
    win = max(1, int(0.0012 * sr))
    if win > 1:
        square_gate = np.convolve(square_gate, np.ones(win) / win, mode="same")
    if x.ndim == 2:
        square_gate = square_gate[:, None]
    return (1.0 - mix) * x + mix * (x * square_gate)


def apply_digital_destruction(x: np.ndarray, sr: int, spec: HyperStyleSpec, preserve_sub_below_hz: float = 0.0) -> np.ndarray:
    if preserve_sub_below_hz > 0.0:
        sub = core.filter_signal(x, preserve_sub_below_hz, "lowpass", sr, 4)
        top = core.filter_signal(x, preserve_sub_below_hz, "highpass", sr, 4)
        top = oversampled_wavefolder(top, drive=spec.drive, fold=spec.fold)
        top = frequency_shift_ssb(top, sr, spec.freq_shift_hz * 0.35)
        top = bitcrush_and_decimate(top, spec.crush_bits, spec.sr_div)
        top = core.filter_signal(top, preserve_sub_below_hz, "highpass", sr, 3)
        out = sub + 0.78 * glitch_gate(top, sr, spec.gate_hz, mix=0.45)
    else:
        y = oversampled_wavefolder(x, drive=spec.drive, fold=spec.fold)
        y = frequency_shift_ssb(y, sr, spec.freq_shift_hz)
        y = bitcrush_and_decimate(y, spec.crush_bits, spec.sr_div)
        out = glitch_gate(y, sr, spec.gate_hz, mix=0.55)
    return core.filter_signal(out, 22.0, "highpass", sr, 2)


# --- Five Signature Hyper Series Generators ---

def gen_hypertrap_nuclear_808(sr: int, root_hz: float = 48.0, spec: HyperStyleSpec | None = None) -> tuple[np.ndarray, float]:
    """48 Hz sub, 180->48 Hz @ 28 ms, 550 ms amp tau, parallel clean sub + wavefolded saturation."""
    st = spec or HYPER_20_STYLES["hypertrap"]
    t = np.arange(int(2.2 * sr)) / sr
    freq = root_hz + (180.0 - root_hz) * np.exp(-t / 0.028)
    if st.rubber > 0.2:
        freq += 8.0 * st.rubber * np.sin(2.0 * np.pi * 13.0 * t) * np.exp(-t / 0.11)
    phase = 2.0 * np.pi * np.cumsum(freq) / sr
    raw = (0.82 * np.sin(phase) + 0.18 * (2.0 / np.pi) * np.arcsin(np.sin(phase))) * (1.0 - np.exp(-t / 0.0006)) * np.exp(-t / 0.550)
    out = apply_digital_destruction(raw, sr, st, preserve_sub_below_hz=115.0)
    return np.column_stack((out, out)), root_hz


def gen_hyperpop_bubble_kick(sr: int, spec: HyperStyleSpec | None = None) -> np.ndarray:
    """110 -> 62 Hz springy rubber pitch drop @ 45 ms, tri/sine morph, resonant plastic FM click."""
    st = spec or HYPER_20_STYLES["hyperpop"]
    t = np.arange(int(0.42 * sr)) / sr
    bounce_wobble = 14.0 * st.rubber * np.sin(2.0 * np.pi * 24.0 * t) * np.exp(-t / 0.065)
    freq = 62.0 + (110.0 - 62.0) * np.exp(-t / 0.045) + bounce_wobble
    phase = 2.0 * np.pi * np.cumsum(freq) / sr
    body = (0.55 * np.sin(phase) + 0.45 * (2.0 / np.pi) * np.arcsin(np.sin(phase))) * (1.0 - np.exp(-t / 0.0003)) * np.exp(-t / 0.125)

    # Plastic FM resonant click
    click_mod = np.sin(2.0 * np.pi * 1420.0 * t) * 2.6 * np.exp(-t / 0.006)
    plastic_click = np.sin(2.0 * np.pi * 620.0 * t + click_mod) * np.exp(-t / 0.010) * 0.38
    mono = apply_digital_destruction(body + plastic_click, sr, st, preserve_sub_below_hz=120.0)
    return np.column_stack((mono, mono))


def gen_glitch_glass_snare(sr: int, rng: np.random.Generator, spec: HyperStyleSpec | None = None) -> np.ndarray:
    """220 Hz body, 2.8 kHz band-passed noise, short metallic partials, gated tail."""
    st = spec or HYPER_20_STYLES["hyperpop"]
    t = np.arange(int(0.38 * sr)) / sr
    body = core.osc(220.0 + 140.0 * np.exp(-t / 0.009), sr) * np.exp(-t / 0.038) * 0.62

    glass = sum(
        (0.6 ** idx) * np.sin(2.0 * np.pi * f0 * st.bright * t)
        for idx, f0 in enumerate((1320.0, 2110.0, 3490.0, 5180.0))
        if f0 * st.bright < sr * 0.44
    ) * np.exp(-t / 0.026) * 0.34

    sos_bp = butter(2, [1900.0, min(4200.0, sr * 0.42)], btype="bandpass", fs=sr, output="sos")
    noise_l = sosfilt(sos_bp, rng.standard_normal(len(t))) * np.exp(-t / 0.085) * 0.85
    noise_r = sosfilt(sos_bp, rng.standard_normal(len(t))) * np.exp(-t / 0.085) * 0.85

    # Hard crisp gate at 155 ms
    tail_gate = np.where(t < 0.155, 1.0, np.exp(-(t - 0.155) / 0.006))[:, None]
    stereo = np.column_stack((body + glass + noise_l, body + glass + noise_r)) * tail_gate
    return apply_digital_destruction(stereo, sr, st)


def gen_hyperplugg_crystal_hat(sr: int, rng: np.random.Generator, spec: HyperStyleSpec | None = None) -> np.ndarray:
    """6 inharmonic oscillators, 9 kHz high-pass, 32 ms decay tau."""
    st = spec or HYPER_20_STYLES["hyperplugg"]
    t = np.arange(int(0.24 * sr)) / sr
    partials = (3120.0, 4290.0, 5680.0, 7190.0, 8840.0, 10650.0)
    metal = np.zeros_like(t)
    for idx, f0 in enumerate(partials):
        freq = min(f0 * min(st.bright, 1.25), sr * 0.45)
        metal += (0.72 ** idx) * np.sin(2.0 * np.pi * freq * t + idx * 0.6)
    air = rng.uniform(-1.0, 1.0, len(t)) * 0.22
    hp_cut = min(9000.0, sr * 0.40)
    filtered = core.filter_signal((metal / 3.5 + air) * (1.0 - np.exp(-t / 0.0002)) * np.exp(-t / 0.032), hp_cut, "highpass", sr, 3)
    mono = oversampled_wavefolder(filtered, drive=max(1.1, st.drive * 0.45), fold=st.fold * 0.4)
    return np.column_stack((mono * 0.96, mono * 1.04))


def gen_digital_alien_perc(sr: int, spec: HyperStyleSpec | None = None) -> np.ndarray:
    """950 Hz FM carrier, 3.2 modulator ratio, falling FM index, 120 ms decay tau."""
    st = spec or HYPER_20_STYLES["hypertrap"]
    t = np.arange(int(0.55 * sr)) / sr
    carrier_hz = 950.0 * (1.0 + 0.25 * st.rubber * np.exp(-t / 0.03))
    mod_hz = np.minimum(carrier_hz * 3.2, sr * 0.44)
    fm_index = 4.2 * np.exp(-t / 0.042) + 0.15
    mod_sig = np.sin(2.0 * np.pi * np.cumsum(mod_hz) / sr) * fm_index
    sig = np.sin(2.0 * np.pi * np.cumsum(carrier_hz) / sr + mod_sig) * (1.0 - np.exp(-t / 0.0003)) * np.exp(-t / 0.120)
    mono = apply_digital_destruction(sig, sr, st)
    return np.column_stack((mono, mono))


def interpolate_hyper_dna(morph_0_to_1: float) -> HyperStyleSpec:
    """Interpolates across Plugg (0.0) -> Rage (0.25) -> Hypertrap (0.50) -> Hyperpop (0.75) -> Digicore (1.0)."""
    pos = float(np.clip(morph_0_to_1, 0.0, 1.0)) * (len(DNA_ANCHORS) - 1)
    lo_idx = min(int(np.floor(pos)), len(DNA_ANCHORS) - 2)
    frac = pos - lo_idx
    a = DNA_ANCHORS[lo_idx][1]
    b = DNA_ANCHORS[lo_idx + 1][1]

    def lerp(x: float, y: float) -> float:
        return float(x + frac * (y - x))

    return HyperStyleSpec(
        id=f"morph_{int(round(morph_0_to_1 * 100)):03d}",
        label=f"HyperDNA {int(round(morph_0_to_1 * 100))}%",
        drive=lerp(a.drive, b.drive),
        fold=lerp(a.fold, b.fold),
        crush_bits=int(round(lerp(float(a.crush_bits), float(b.crush_bits)))),
        sr_div=int(round(lerp(float(a.sr_div), float(b.sr_div)))),
        gate_hz=lerp(a.gate_hz, b.gate_hz),
        freq_shift_hz=lerp(a.freq_shift_hz, b.freq_shift_hz),
        rubber=lerp(a.rubber, b.rubber),
        bright=lerp(a.bright, b.bright),
    )


def render_morph_five_pack(voice_kind: str, sr: int = 48000, seed: int = 808999) -> dict[str, np.ndarray]:
    """Renders 1 drum identity across all 5 Hyper DNA anchors (Plugg, Rage, Hypertrap, Hyperpop, Digicore)."""
    renders: dict[str, np.ndarray] = {}
    for idx, (anchor_name, spec) in enumerate(DNA_ANCHORS):
        rng = np.random.default_rng(seed)  # identical seed preserves core transient identity
        if voice_kind == "808":
            sig, _ = gen_hypertrap_nuclear_808(sr, root_hz=48.0, spec=spec)
        elif voice_kind == "kick":
            sig = gen_hyperpop_bubble_kick(sr, spec=spec)
        elif voice_kind == "snare":
            sig = gen_glitch_glass_snare(sr, rng, spec=spec)
        elif voice_kind == "hat":
            sig = gen_hyperplugg_crystal_hat(sr, rng, spec=spec)
        else:
            sig = gen_digital_alien_perc(sr, spec=spec)
        renders[anchor_name] = core.master(brick.trim_below_db(sig, sr), sr, -1.0)
    return renders


def export_hyper_series_showcase(out_dir: Path, sr: int = 48000, seed: int = 808999, make_zip: bool = True) -> dict:
    """Exports all 20 Hyper Series styles + the 5 Release Wave folders + 5-way Hyper DNA Morph set."""
    out_dir.mkdir(parents=True, exist_ok=True)
    written: list[Path] = []
    rows = []

    release_waves = ("hypertrap", "hyperpop", "hyperplugg", "digicore", "hexd")
    for wave_id in release_waves:
        spec = HYPER_20_STYLES[wave_id]
        rng = np.random.default_rng(seed + sum(wave_id.encode()))
        s808, _ = gen_hypertrap_nuclear_808(sr, 48.0, spec)
        skick = gen_hyperpop_bubble_kick(sr, spec)
        ssnare = gen_glitch_glass_snare(sr, rng, spec)
        shat = gen_hyperplugg_crystal_hat(sr, rng, spec)
        sperc = gen_digital_alien_perc(sr, spec)

        for tag, buf in (("Nuclear808_G1", s808), ("BubbleKick", skick), ("GlitchGlassSnare", ssnare), ("CrystalHat", shat), ("AlienPerc", sperc)):
            mastered = core.master(brick.trim_below_db(buf, sr), sr, -1.0)
            rel = Path("01_Release_Waves") / wave_id / f"DL_HYPER_{wave_id}_{tag}.wav"
            dest = out_dir / rel
            dest.parent.mkdir(parents=True, exist_ok=True)
            sf.write(dest, mastered, sr, subtype="PCM_24")
            written.append(dest)
            rows.append({"file": rel.as_posix(), "style": wave_id, "true_peak_dbtp": round(core.true_peak_db(mastered), 3)})

    # Render signature hit across all 20 Hyper Series styles
    for idx, (style_id, spec) in enumerate(HYPER_20_STYLES.items(), start=1):
        rng = np.random.default_rng(seed + idx * 53)
        sig = gen_glitch_glass_snare(sr, rng, spec) if idx % 2 == 0 else gen_hyperpop_bubble_kick(sr, spec)
        mastered = core.master(brick.trim_below_db(sig, sr), sr, -1.0)
        rel = Path("02_All_20_Hyper_Styles") / f"{idx:02d}_DL_{style_id}.wav"
        dest = out_dir / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        sf.write(dest, mastered, sr, subtype="PCM_24")
        written.append(dest)
        rows.append({"file": rel.as_posix(), "style": style_id, "true_peak_dbtp": round(core.true_peak_db(mastered), 3)})

    # Render 5-way Hyper DNA Morph demonstration (same seed/note morphed across 5 genres)
    for anchor_name, buf in render_morph_five_pack("snare", sr=sr, seed=seed).items():
        rel = Path("03_Hyper_DNA_Morph") / f"DL_HyperDNA_Snare_{anchor_name}.wav"
        dest = out_dir / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        sf.write(dest, buf, sr, subtype="PCM_24")
        written.append(dest)
        rows.append({"file": rel.as_posix(), "style": f"dna_{anchor_name}", "true_peak_dbtp": round(core.true_peak_db(buf), 3)})

    manifest_path = out_dir / "manifest.json"
    manifest = {
        "engine": ENGINE,
        "sample_rate": sr,
        "seed": seed,
        "styles_count": len(HYPER_20_STYLES),
        "hypertrap_vol1_target": HYPERTRAP_400_SPEC,
        "hyperpop_vol1_target": HYPERPOP_400_SPEC,
        "count": len(rows),
        "files": rows,
    }
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")

    if make_zip:
        zip_path = out_dir / "DiskLordz_Hyper_Series_Pack.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for item in written:
                zf.write(item, arcname=item.relative_to(out_dir).as_posix())
            zf.write(manifest_path, arcname="manifest.json")
        manifest["zip"] = zip_path.name
    return manifest


def main() -> None:
    ap = argparse.ArgumentParser(description=ENGINE)
    ap.add_argument("--sr", type=int, choices=(44100, 48000, 96000), default=48000)
    ap.add_argument("--seed", type=int, default=808999)
    ap.add_argument("--out", default="DiskLordz_Hyper_Series_Kit")
    args = ap.parse_args()
    manifest = export_hyper_series_showcase(Path(args.out), args.sr, args.seed)
    print(f"Exported {manifest['count']} Hyper Series WAVs across {manifest['styles_count']} styles to {args.out}")


if __name__ == "__main__":
    main()
