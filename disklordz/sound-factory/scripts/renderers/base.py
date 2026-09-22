"""Abstract renderer contract for multisample factory engines."""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import Any


class Renderer(ABC):
    @property
    @abstractmethod
    def name(self) -> str: ...

    @property
    @abstractmethod
    def sample_rate(self) -> int: ...

    @abstractmethod
    def render_note(
        self,
        midi_note: int,
        velocity: int,
        note_duration_sec: float,
        total_duration_sec: float,
    ) -> Any: ...
