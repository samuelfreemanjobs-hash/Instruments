"""Shared DawDreamer multisample session rendering (one plugin load per grid)."""
from __future__ import annotations

import os
from pathlib import Path

import dawdreamer as daw
from scipy.io import wavfile

from dawdreamer_fx import load_fx_preset, resolve_fx_plugin
from map_lib import finalize_session_dir


def normalise_vst3_path(path: Path) -> Path:
    p = path.resolve()
    if p.suffix == ".vst3" and p.is_file():
        return p.parent
    return p


def load_preset(synth, preset_path: Path | None) -> None:
    if preset_path is None or not preset_path.is_file():
        return
    path_str = str(preset_path.resolve())
    suffix = preset_path.suffix.lower()
    if suffix == ".vstpreset":
        synth.load_vst3_preset(path_str)
    else:
        synth.load_preset(path_str)


def render_session_grid(
    plugin: Path,
    dest: Path,
    *,
    program: int,
    note_start: int,
    note_end: int,
    note_step: int,
    velocity: int,
    seconds: float,
    sample_rate: int,
    block_size: int,
    preset_path: Path | None,
    instrument_label: str,
    source_plugin: str,
    instrument_id: str | None = None,
    write_sfz: bool = False,
    fx_plugin: Path | None = None,
    fx_preset_path: Path | None = None,
) -> Path:
    dest.mkdir(parents=True, exist_ok=True)
    manifest = dest / "manifest.tsv"
    plugin_path = str(normalise_vst3_path(plugin).resolve())
    fx_path = resolve_fx_plugin(fx_plugin)
    fx_path_str = str(fx_path.resolve()) if fx_path else ""
    fx_preset_col = str(fx_preset_path) if fx_preset_path else ""
    engine_label = "dawdreamer+osirus_fx" if fx_path else "dawdreamer"

    header = (
        "instrument_label\tsource_plugin\tplugin_path\tprogram\tpreset_path\tnote\tvelocity\t"
        "seconds\tsample_rate\tengine\tfx_plugin_path\tfx_preset_path\twav\n"
    )
    manifest.write_text(header, encoding="utf-8")

    engine = daw.RenderEngine(sample_rate, block_size)
    synth = engine.make_plugin_processor("synth", str(normalise_vst3_path(plugin)))
    if fx_path:
        fx = engine.make_plugin_processor("osirus_fx", str(normalise_vst3_path(fx_path)))
        load_fx_preset(fx, fx_preset_path)
        engine.load_graph([(synth, []), (fx, [synth.get_name()])])
    else:
        engine.load_graph([(synth, [])])

    if program >= 0:
        try:
            synth.set_program(program)
        except AttributeError:
            pass
    load_preset(synth, preset_path)

    preset_col = str(preset_path) if preset_path else ""

    for note in range(note_start, note_end + 1, note_step):
        synth.clear_midi()
        synth.add_midi_note(note, velocity, 0.0, seconds)
        engine.render(seconds)
        audio = engine.get_audio()
        if audio.size == 0:
            raise RuntimeError(f"Empty render for note {note}")

        wav_path = dest / f"note_{note:03d}_vel_{velocity:03d}.wav"
        wavfile.write(str(wav_path), sample_rate, audio.T)

        with manifest.open("a", encoding="utf-8") as mf:
            mf.write(
                f"{instrument_label}\t{source_plugin}\t{plugin_path}\t{program}\t{preset_col}\t{note}\t"
                f"{velocity}\t{seconds}\t{sample_rate}\t{engine_label}\t{fx_path_str}\t{fx_preset_col}\t{wav_path}\n"
            )

    if write_sfz or os.environ.get("MULTISAMPLE_WRITE_SFZ", "").strip().lower() in (
        "1",
        "true",
        "yes",
    ):
        write_sfz = True

    map_path = finalize_session_dir(dest, instrument_id=instrument_id, write_sfz=write_sfz)
    print(f"Wrote {map_path}")
    return manifest
