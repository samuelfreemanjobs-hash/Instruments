from __future__ import annotations

import json
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Any

from vst_factory.modules import render_template


class ModelBackend(ABC):
    @abstractmethod
    def complete(self, system: str, user: str) -> str: ...


class ExportOnlyBackend(ModelBackend):
    """Phase 1: store prompts for Cursor; no external API."""

    def complete(self, system: str, user: str) -> str:
        return f"[export-only]\n\n## System\n{system}\n\n## User\n{user}"


AGENT_FILES = {
    "architect": "agents/architect.md",
    "dsp": "agents/dsp.md",
    "cpp": "agents/cpp.md",
    "gui": "agents/gui.md",
    "midi": "agents/midi.md",
    "build": "agents/build.md",
    "qa": "agents/qa.md",
    "repair": "agents/repair.md",
}


def package_root() -> Path:
    return Path(__file__).resolve().parents[1]


def load_agent_prompt(role: str) -> str:
    rel = AGENT_FILES.get(role)
    if not rel:
        raise KeyError(role)
    path = package_root() / rel
    return path.read_text(encoding="utf-8")


def export_cursor_impl_prompt(spec: dict[str, Any]) -> str:
    return render_template(
        "prompts/cursor-impl.md",
        plugin_name=spec["plugin_name"],
        plugin_class=spec["plugin_class"],
        prompt=spec.get("prompt", ""),
        spec_json=json.dumps(spec, indent=2),
        dsp_modules=", ".join(spec.get("dsp_modules", [])),
        gui_modules=", ".join(spec.get("gui_modules", [])),
    )


def default_backend() -> ModelBackend:
    return ExportOnlyBackend()
