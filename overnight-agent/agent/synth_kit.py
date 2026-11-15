"""Deterministic procedural drum kit synthesizer (numpy, no samples)."""
from __future__ import annotations

import json
import math
import wave
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Dict, List, Tuple

import numpy as np

SR = 44100


@dataclass
class KitParams:
    seed: int = 0x0D15C1AB
    kick_decay: float = 0.28
    kick_pitch_start: float = 160.0
    kick_pitch_end: float = 48.0
    kick_drive: float = 2.2
    snare_decay: float = 0.18
    snare_tone: float = 210.0
    snare_noise: float = 0.65
    hat_decay: float = 0.045
    open_hat_decay: float = 0.22
    clap_bursts: int = 3
    tom_low_hz: float = 90.0
    tom_high_hz: float = 140.0
    perc_hz: float = 680.0
    drive_master: float = 1.15
    noise_mix: float = 0.35

    def clamp(self) -> "KitParams":
        self.kick_decay = float(np.clip(self.kick_decay, 0.08, 0.7))
        self.kick_pitch_start = float(np.clip(self.kick_pitch_start, 80.0, 280.0))
        self.kick_pitch_end = float(np.clip(self.kick_pitch_end, 35.0, 80.0))
        self.kick_drive = float(np.clip(self.kick_drive, 0.8, 4.5))
        self.snare_decay = float(np.clip(self.snare_decay, 0.06, 0.4))
        self.snare_tone = float(np.clip(self.snare_tone, 140.0, 320.0))
        self.snare_noise = float(np.clip(self.snare_noise, 0.2, 0.95))
        self.hat_decay = float(np.clip(self.hat_decay, 0.02, 0.12))
        self.open_hat_decay = float(np.clip(self.open_hat_decay, 0.1, 0.45))
        self.clap_bursts = int(np.clip(self.clap_bursts, 2, 5))
        self.tom_low_hz = float(np.clip(self.tom_low_hz, 60.0, 120.0))
        self.tom_high_hz = float(np.clip(self.tom_high_hz, 110.0, 200.0))
        self.perc_hz = float(np.clip(self.perc_hz, 400.0, 1200.0))
        self.drive_master = float(np.clip(self.drive_master, 0.7, 2.0))
        self.noise_mix = float(np.clip(self.noise_mix, 0.05, 0.8))
        return self


VOICE_MAP: Dict[str, int] = {
    "kick": 36,
    "snare": 38,
    "clap": 39,
    "closed_hat": 42,
    "open_hat": 46,
    "tom_low": 41,
    "tom_high": 45,
    "perc": 47,
}


def _rng(seed: int) -> np.random.Generator:
    return np.random.default_rng(seed & 0xFFFFFFFF)


def _env(n: int, decay: float, attack: float = 0.002) -> np.ndarray:
    t = np.arange(n) / SR
    att = np.clip(t / max(attack, 1e-4), 0.0, 1.0)
    return att * np.exp(-t / max(decay, 1e-4))


def _soft(x: np.ndarray, drive: float) -> np.ndarray:
    return np.tanh(x * drive).astype(np.float32)


def synth_kick(p: KitParams, velocity: float = 1.0) -> np.ndarray:
    n = int(SR * min(1.2, p.kick_decay * 3.5))
    t = np.arange(n) / SR
    freq = p.kick_pitch_end + (p.kick_pitch_start - p.kick_pitch_end) * np.exp(-t / 0.022)
    phase = 2.0 * math.pi * np.cumsum(freq) / SR
    body = np.sin(phase)
    click = _rng(p.seed + 1).standard_normal(n) * np.exp(-t / 0.004) * 0.25
    return _soft((body + click) * _env(n, p.kick_decay) * velocity, p.kick_drive)


def synth_snare(p: KitParams, velocity: float = 1.0) -> np.ndarray:
    n = int(SR * min(0.8, p.snare_decay * 4.0))
    t = np.arange(n) / SR
    tone = np.sin(2 * math.pi * p.snare_tone * t) * np.exp(-t / (p.snare_decay * 0.45))
    noise = _rng(p.seed + 2).standard_normal(n)
    # crude HP
    noise = np.diff(noise, prepend=noise[0])
    noise *= np.exp(-t / p.snare_decay)
    mix = tone * (1.0 - p.snare_noise) + noise * p.snare_noise
    return _soft(mix * _env(n, p.snare_decay, 0.001) * velocity, 1.6)


def synth_hat(p: KitParams, open_hat: bool, velocity: float = 1.0) -> np.ndarray:
    decay = p.open_hat_decay if open_hat else p.hat_decay
    n = int(SR * min(1.5, decay * 5.0))
    t = np.arange(n) / SR
    ratios = np.array([205.3, 304.4, 369.6, 522.7, 540.0, 800.0])
    metallic = np.zeros(n)
    for r in ratios:
        metallic += np.sign(np.sin(2 * math.pi * r * t))
    metallic /= len(ratios)
    hp = np.diff(metallic, prepend=metallic[0])
    return _soft(hp * _env(n, decay, 0.0008) * velocity * 0.7, 1.2)


def synth_clap(p: KitParams, velocity: float = 1.0) -> np.ndarray:
    n = int(SR * 0.4)
    t = np.arange(n) / SR
    noise = _rng(p.seed + 3).standard_normal(n)
    lp = np.zeros(n)
    for i in range(1, n):
        lp[i] = lp[i - 1] + 0.28 * (noise[i] - lp[i - 1])
    env = np.zeros(n)
    for b in range(p.clap_bursts):
        bt = t - b * 0.011
        env += np.where(bt >= 0, 0.55 * np.exp(-np.maximum(bt, 0) / 0.008), 0.0)
    env += np.where(t >= 0.033, 0.85 * np.exp(-(t - 0.033) / 0.12), 0.0)
    return _soft(lp * env * velocity, 1.8)


def synth_tom(p: KitParams, high: bool, velocity: float = 1.0) -> np.ndarray:
    hz = p.tom_high_hz if high else p.tom_low_hz
    n = int(SR * 0.55)
    t = np.arange(n) / SR
    freq = hz + 80.0 * np.exp(-t / 0.02)
    phase = 2.0 * math.pi * np.cumsum(freq) / SR
    return _soft(np.sin(phase) * _env(n, 0.22 if high else 0.32) * velocity, 1.5)


def synth_perc(p: KitParams, velocity: float = 1.0) -> np.ndarray:
    n = int(SR * 0.2)
    t = np.arange(n) / SR
    s = np.sin(2 * math.pi * p.perc_hz * t) + 0.5 * np.sin(2 * math.pi * p.perc_hz * 2.4 * t)
    return _soft(s * _env(n, 0.04) * velocity, 1.7)


def write_wav_mono(path: Path, audio: np.ndarray) -> None:
    audio = np.asarray(audio, dtype=np.float32)
    peak = float(np.max(np.abs(audio))) if audio.size else 0.0
    if peak > 1e-8:
        audio = audio / peak * 0.95
    pcm = np.clip(audio * 32767.0, -32768, 32767).astype("<i2")
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(pcm.tobytes())


def generate_kit(out_dir: Path, params: KitParams) -> dict:
    params = params.clamp()
    rng = _rng(params.seed)
    # slight per-voice seed jitter for uniqueness
    voices: List[Tuple[str, np.ndarray]] = [
        ("kick", synth_kick(params, 1.0)),
        ("kick_soft", synth_kick(params, 0.55)),
        ("snare", synth_snare(params, 1.0)),
        ("snare_soft", synth_snare(params, 0.5)),
        ("clap", synth_clap(params, 1.0)),
        ("closed_hat", synth_hat(params, False, 0.9)),
        ("open_hat", synth_hat(params, True, 0.85)),
        ("tom_low", synth_tom(params, False, 0.9)),
        ("tom_high", synth_tom(params, True, 0.9)),
        ("perc", synth_perc(params, 0.85)),
    ]
    files = []
    for name, audio in voices:
        # master drive + tiny noise grit
        audio = _soft(audio * params.drive_master, 1.0)
        if params.noise_mix > 0:
            grit = rng.standard_normal(len(audio)) * params.noise_mix * 0.02
            audio = audio + grit.astype(np.float32) * np.abs(audio)
        rel = f"{name}.wav"
        write_wav_mono(out_dir / rel, audio)
        note = VOICE_MAP.get(name.split("_")[0] if name.endswith("_soft") else name, 0)
        if name.endswith("_soft"):
            base = name.replace("_soft", "")
            note = VOICE_MAP.get(base, 0)
        files.append({"file": rel, "midi": note, "velocity": "soft" if name.endswith("_soft") else "hard"})

    manifest = {
        "schema": 1,
        "sample_rate": SR,
        "bit_depth": 16,
        "channels": 1,
        "params": asdict(params),
        "voices": files,
        "note_map": VOICE_MAP,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    return manifest
