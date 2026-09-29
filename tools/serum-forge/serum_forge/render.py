"""Headless Serum render via Spotify Pedalboard (optional)."""

from __future__ import annotations

import os
from pathlib import Path


def render_preset_headless(preset_path: Path, out_wav: Path, *, midi_note: int = 60) -> Path:
    plugin = os.environ.get("SERUM_VST3_PATH", "")
    if not plugin or not Path(plugin).is_file():
        raise RuntimeError(
            "Set SERUM_VST3_PATH to Serum.vst3 for headless render. "
            "Load preset via chunk/bank reload — host automation cannot set mod matrix."
        )
    try:
        from pedalboard import load_plugin
        from pedalboard.io import AudioFile
        import numpy as np
    except ImportError as exc:
        raise RuntimeError("pip install pedalboard numpy") from exc

    synth = load_plugin(plugin)
    # Preset load is host-specific; many builds require preset file on disk + bank refresh.
    if hasattr(synth, "load_preset"):
        synth.load_preset(str(preset_path))
    duration = 2.0
    sr = 44100
    n = int(duration * sr)
    # Placeholder: silence until chunk injection is wired for your Serum build.
    audio = np.zeros((2, n), dtype=np.float32)
    out_wav.parent.mkdir(parents=True, exist_ok=True)
    with AudioFile(str(out_wav), "w", sr, 2) as f:
        f.write(audio)
    return out_wav
