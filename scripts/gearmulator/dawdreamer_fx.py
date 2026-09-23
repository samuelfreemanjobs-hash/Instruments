"""Osirus FX (or any VST3) serial chain helpers for DawDreamer."""
from __future__ import annotations

import os
from pathlib import Path

import dawdreamer as daw
import numpy as np
from scipy.io import wavfile


def normalise_vst3_path(path: Path) -> Path:
    p = path.resolve()
    if p.suffix == ".vst3" and p.is_file():
        return p.parent
    return p


def default_osirus_fx_path() -> Path | None:
    for key in ("GEARMULATOR_OSIRUS_FX_VST3", "INSTRUMENTS_OSIRUS_FX_VST3"):
        raw = os.environ.get(key, "").strip()
        if raw:
            p = Path(raw)
            if p.exists():
                return p
    repo = Path(__file__).resolve().parents[2]
    env_file = repo / "gearmulator-lane" / ".gearmulator-osirus-fx.env"
    if env_file.is_file():
        for line in env_file.read_text(encoding="utf-8").splitlines():
            if "GEARMULATOR_OSIRUS_FX_VST3=" in line:
                raw = line.split("=", 1)[1].strip().strip('"')
                p = Path(raw)
                if p.exists():
                    return p
    return None


def always_use_osirus_fx() -> bool:
    if os.environ.get("INSTRUMENTS_ALWAYS_OSIRUS_FX", "").strip().lower() in ("1", "true", "yes"):
        return True
    return os.environ.get("INSTRUMENTS_OSIRUS_FX_DEFAULT", "").strip().lower() in ("1", "true", "yes")


def resolve_fx_plugin(explicit: Path | None) -> Path | None:
    if explicit is not None and explicit.exists():
        return explicit
    if always_use_osirus_fx():
        return default_osirus_fx_path()
    return None


def load_fx_preset(fx, preset_path: Path | None) -> None:
    if preset_path is None or not preset_path.is_file():
        return
    path_str = str(preset_path.resolve())
    if preset_path.suffix.lower() == ".vstpreset":
        fx.load_vst3_preset(path_str)
    else:
        fx.load_preset(path_str)


def render_synth_through_fx(
    synth_plugin: Path,
    fx_plugin: Path,
    seconds: float,
    sample_rate: int,
    block_size: int,
    *,
    program: int = 0,
    synth_preset: Path | None = None,
    fx_preset: Path | None = None,
    midi_note: int = 60,
    velocity: int = 100,
) -> np.ndarray:
    engine = daw.RenderEngine(sample_rate, block_size)
    synth = engine.make_plugin_processor("synth", str(normalise_vst3_path(synth_plugin)))
    fx = engine.make_plugin_processor("osirus_fx", str(normalise_vst3_path(fx_plugin)))
    engine.load_graph([(synth, []), (fx, [synth.get_name()])])

    if program >= 0:
        try:
            synth.set_program(program)
        except AttributeError:
            pass
    if synth_preset:
        load_fx_preset(synth, synth_preset)
    load_fx_preset(fx, fx_preset)

    synth.clear_midi()
    synth.add_midi_note(midi_note, velocity, 0.0, seconds)
    engine.render(seconds)
    audio = engine.get_audio()
    if audio.size == 0:
        raise RuntimeError("Empty render from synth+FX graph")
    return audio


def process_wav_through_fx(
    wav_in: Path,
    wav_out: Path,
    fx_plugin: Path,
    *,
    fx_preset: Path | None = None,
    sample_rate: int = 48000,
    block_size: int = 512,
) -> None:
    sr, data = wavfile.read(str(wav_in))
    if sr != sample_rate:
        raise ValueError(f"Expected sample_rate {sample_rate}, got {sr} from {wav_in}")
    if data.ndim == 1:
        data = np.stack([data, data])
    elif data.shape[0] != 2 and data.shape[1] == 2:
        data = data.T

    engine = daw.RenderEngine(sample_rate, block_size)
    playback = engine.make_playback_processor("dry", data.astype(np.float32) / np.iinfo(data.dtype).max)
    fx = engine.make_plugin_processor("osirus_fx", str(normalise_vst3_path(fx_plugin)))
    engine.load_graph([(playback, []), (fx, [playback.get_name()])])
    load_fx_preset(fx, fx_preset)

    duration = data.shape[1] / sample_rate
    engine.render(duration)
    out = engine.get_audio()
    wavfile.write(str(wav_out), sample_rate, out.T)
