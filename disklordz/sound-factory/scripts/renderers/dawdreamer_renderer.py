"""Host a VST/VST3/CLAP plugin via DawDreamer for offline multisample renders."""

from __future__ import annotations

from typing import Any

import numpy as np

from .base import Renderer


class DawDreamerRenderer(Renderer):
    """
    Renderer backed by DawDreamer (https://github.com/DBraun/DawDreamer).

    Used by the MPC-agent / sound-factory path when a third-party VSTi should
    drive multisample WAVs instead of Rev2TrapOfflineRender or trap_synth.
    """

    def __init__(
        self,
        plugin_path: str,
        sample_rate: int = 44100,
        buffer_size: int = 512,
        processor_name: str = "factory_synth",
    ):
        try:
            import dawdreamer as daw
        except ImportError as e:
            raise ImportError(
                "dawdreamer is not installed. "
                "pip install -r disklordz/sound-factory/requirements-dawdreamer.txt"
            ) from e

        self._sample_rate = sample_rate
        self._buffer_size = buffer_size
        self._plugin_path = plugin_path
        self._engine = daw.RenderEngine(sample_rate, buffer_size)
        self._synth = self._engine.make_plugin_processor(processor_name, plugin_path)
        self._engine.load_graph([(self._synth, [])])

    @property
    def name(self) -> str:
        return "dawdreamer"

    @property
    def sample_rate(self) -> int:
        return self._sample_rate

    @property
    def synth(self) -> Any:
        return self._synth

    def parameter_descriptions(self) -> list[dict[str, Any]]:
        return self._synth.get_parameters_description()

    def get_parameter(self, index: int) -> float:
        return self._synth.get_parameter(index)

    def set_parameter(self, index: int, value: float) -> None:
        self._synth.set_parameter(index, float(value))

    def render_note(
        self,
        midi_note: int,
        velocity: int,
        note_duration_sec: float,
        total_duration_sec: float,
    ) -> np.ndarray:
        self._synth.clear_midi()
        self._synth.add_midi_note(midi_note, velocity, 0.0, note_duration_sec)
        self._engine.render(total_duration_sec)
        return self._engine.get_audio()
