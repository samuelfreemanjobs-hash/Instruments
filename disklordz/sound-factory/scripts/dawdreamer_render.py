"""Render multisample zones via DawDreamer + a user-supplied VST/VST3/CLAP."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path

import numpy as np

from factory_qa import analyze_wav
from post_render import apply_limiter
from renderers.dawdreamer_renderer import DawDreamerRenderer
from trap_synth import default_multisample_roots
from wav_util import SAMPLE_RATE, write_mono_wav

NOTE_DURATION_S = 2.4
TOTAL_DURATION_S = 2.8


def resolve_vst_path(explicit: Path | None = None) -> Path:
    if explicit and explicit.is_file():
        return explicit
    env = os.environ.get("FACTORY_VST_PATH") or os.environ.get("DAWDREAMER_VST_PATH")
    if env:
        p = Path(env)
        if p.is_file():
            return p
    raise FileNotFoundError(
        "No VST plugin for DawDreamer. Pass --vst-path or set FACTORY_VST_PATH "
        "(e.g. build/Rev2Trap/Rev2Trap.vst3/Contents/x86_64-linux/Rev2Trap.so on Linux)."
    )


def stereo_to_mono(audio: np.ndarray) -> list[float]:
    if audio.ndim == 1:
        return audio.astype(np.float64).tolist()
    if audio.shape[0] == 2:
        return np.mean(audio, axis=0).astype(np.float64).tolist()
    return np.mean(audio, axis=0).astype(np.float64).tolist()


def render_note_to_wav(
    renderer: DawDreamerRenderer,
    wav: Path,
    midi_note: int,
    velocity: int = 110,
) -> str:
    audio = renderer.render_note(midi_note, velocity, NOTE_DURATION_S, TOTAL_DURATION_S)
    mono = stereo_to_mono(np.asarray(audio))
    sha = write_mono_wav(wav, mono)
    apply_limiter(wav)
    return sha


def build_zones_dawdreamer(
    out_dir: Path,
    instrument_id: str,
    slot: dict,
    vst_path: Path | None = None,
    program: int | None = None,
) -> dict:
    plugin = resolve_vst_path(vst_path)
    renderer = DawDreamerRenderer(str(plugin), sample_rate=SAMPLE_RATE)
    if program is not None and hasattr(renderer.synth, "set_parameter"):
        # Common JUCE pattern: program as parameter 0 — override via slot if needed.
        idx = int(slot.get("programParamIndex", 0))
        try:
            renderer.set_parameter(idx, float(program))
        except Exception:
            pass

    roots = default_multisample_roots()
    zones = []
    for root in roots:
        lo, hi = max(0, root - 1), min(127, root + 1)
        fname = f"{instrument_id}_root{root}.wav"
        rel = f"samples/{fname}"
        wav = out_dir / rel
        wav.parent.mkdir(parents=True, exist_ok=True)
        sha = render_note_to_wav(renderer, wav, root)
        qa = analyze_wav(wav)
        if not qa.pass_qa:
            raise RuntimeError(f"QA failed {wav}: {qa.reasons}")
        zones.append(
            {
                "samplePath": rel,
                "rootKey": root,
                "loKey": lo,
                "hiKey": hi,
                "loVel": 1,
                "hiVel": 127,
                "volumeDb": 0.0,
                "sha256": sha,
            }
        )
    return {
        "format": "DISKLORDZ_INSTRUMENT_MAP",
        "version": 1,
        "instrumentId": instrument_id,
        "name": slot.get("name", instrument_id),
        "productId": slot.get("productId", "REV2-TRAP-128"),
        "sampleRate": SAMPLE_RATE,
        "engine": "DAWDreamer_renderer",
        "vstPath": str(plugin),
        "zones": zones,
        "metadata": {
            "provenance": "dawdreamer_vst",
            "manifestSha256": hashlib.sha256(
                "".join(z["sha256"] for z in zones).encode()
            ).hexdigest(),
        },
    }
