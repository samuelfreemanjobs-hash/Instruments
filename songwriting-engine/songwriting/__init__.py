"""Deterministic MIDI-first songwriting engine."""

from .generator import generate_project, load_config
from .models import Project

__all__ = ["Project", "generate_project", "load_config"]
