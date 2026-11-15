from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


@dataclass
class MidiConfig:
    input_port_substring: str
    output_port_substring: str
    fallback_input_index: int | None
    fallback_output_index: int | None


@dataclass
class DisklordzInboxConfig:
    enabled: bool
    inbox_dir: Path
    poll_seconds: float


@dataclass
class ExtensionsConfig:
    log_pads: bool
    disklordz_inbox: DisklordzInboxConfig


@dataclass
class BoxConfig:
    mode: str
    midi: MidiConfig
    midi_map_path: Path
    extensions: ExtensionsConfig
    config_dir: Path


def _resolve_path(base: Path, raw: str) -> Path:
    p = Path(raw)
    if p.is_absolute():
        return p
    return (base / p).resolve()


def load_box_config(path: Path) -> BoxConfig:
    path = path.resolve()
    data: dict[str, Any] = json.loads(path.read_text(encoding="utf-8"))
    config_dir = path.parent.parent if path.parent.name == "config" else path.parent

    midi_raw = data.get("midi", {})
    ext_raw = data.get("extensions", {})
    inbox_raw = ext_raw.get("disklordzInbox", {})

    return BoxConfig(
        mode=str(data.get("mode", "sidecar")),
        midi=MidiConfig(
            input_port_substring=str(midi_raw.get("inputPortSubstring", "Renaissance")),
            output_port_substring=str(midi_raw.get("outputPortSubstring", "loopMIDI")),
            fallback_input_index=midi_raw.get("fallbackInputIndex"),
            fallback_output_index=midi_raw.get("fallbackOutputIndex"),
        ),
        midi_map_path=_resolve_path(config_dir, str(data.get("midiMapPath", "config/midi-map.template.json"))),
        extensions=ExtensionsConfig(
            log_pads=bool(ext_raw.get("logPads", False)),
            disklordz_inbox=DisklordzInboxConfig(
                enabled=bool(inbox_raw.get("enabled", False)),
                inbox_dir=_resolve_path(config_dir, str(inbox_raw.get("inboxDir", "."))),
                poll_seconds=float(inbox_raw.get("pollSeconds", 5)),
            ),
        ),
        config_dir=config_dir,
    )


def load_midi_map(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))
