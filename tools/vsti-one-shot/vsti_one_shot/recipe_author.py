"""Recipe Author — expands a style brief into 100 one-shot slots (no VST logic)."""

from __future__ import annotations

import hashlib
import re
from typing import Any

SLOT_COUNT = 100

# Root notes for MPC transpose (spread across useful melody range)
ROOT_MIDI_CYCLE = (48, 50, 52, 55, 57, 60, 62, 64, 67, 72)

# Slot archetypes rotate capture length / role hints for the matcher
ARCHETYPES = (
    {"kind": "stab", "max_capture_sec": 4.0, "tags": ["stab", "short", "melodic"]},
    {"kind": "key", "max_capture_sec": 10.0, "tags": ["keys", "melodic", "sustain"]},
    {"kind": "bell", "max_capture_sec": 6.0, "tags": ["bell", "pluck", "melodic"]},
    {"kind": "pad_hit", "max_capture_sec": 12.0, "tags": ["pad", "texture", "melodic"]},
)


def slugify_style(style: str) -> str:
    s = style.lower().strip()
    s = re.sub(r"[^a-z0-9]+", "_", s)
    s = re.sub(r"_+", "_", s).strip("_")
    return s[:48] or "style"


def build_recipe(style: str, *, slot_count: int = SLOT_COUNT) -> dict[str, Any]:
    """Build machine-readable recipe from a natural-language style brief."""
    slug = slugify_style(style)
    style_hash = hashlib.sha256(style.encode()).hexdigest()[:8]

    slots: list[dict[str, Any]] = []
    for i in range(slot_count):
        idx = i + 1
        archetype = ARCHETYPES[i % len(ARCHETYPES)]
        root_midi = ROOT_MIDI_CYCLE[i % len(ROOT_MIDI_CYCLE)]
        slot_id = f"{slug}_{idx:03d}"

        slots.append(
            {
                "id": slot_id,
                "style": style,
                "kind": archetype["kind"],
                "root_midi": root_midi,
                "max_capture_sec": archetype["max_capture_sec"],
                "tags": list(archetype["tags"]) + [slug.replace("_", " ")],
                "intent": f"{style} — {archetype['kind']} one-shot #{idx}",
            }
        )

    return {
        "version": 1,
        "style": style,
        "style_slug": slug,
        "style_hash": style_hash,
        "slot_count": slot_count,
        "defaults": {
            "sample_rate": 44100,
            "velocity": 100,
            "channels": "mono",
            "normalize_peak_db": -1.0,
            "tail_silence_db": -60.0,
            "max_capture_sec": 8.0,
        },
        "slots": slots,
    }
