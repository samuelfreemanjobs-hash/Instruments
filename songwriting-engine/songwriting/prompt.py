"""Small deterministic parser for natural-language composition briefs."""

from __future__ import annotations

import re
from typing import Any

from .generator import DEFAULT_SECTIONS, STYLE_PROFILES
from .theory import parse_chord, pitch_class

_PITCH_NAMES = ("C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B")
_KEY_MODE = re.compile(r"\b(?:in\s+)?([A-G](?:#|b)?)\s+(natural\s+)?(minor|major|dorian|mixolydian|blues)\b", re.I)
_CHORD_TOKEN = re.compile(r"\b[A-G](?:#|b)?(?:maj|min|m|dim|aug|sus2|sus4)?(?:13|11|9|7|6)?(?:add9)?(?:[#b]\d+)?\b")


def _default_progression(style: str, key: str) -> list[str]:
    root = pitch_class(key)
    names = [_PITCH_NAMES[(root + interval) % 12] for interval in (0, 5, 8, 10)]
    if style == "quiet_storm":
        return [f"{names[0]}maj9", f"{_PITCH_NAMES[(root + 9) % 12]}m9", f"{names[1]}m9", f"{_PITCH_NAMES[(root + 7) % 12]}13"]
    if style == "gospel_rnb":
        return [f"{names[0]}maj9", f"{_PITCH_NAMES[(root + 9) % 12]}7", f"{names[1]}m9", f"{_PITCH_NAMES[(root + 7) % 12]}13"]
    if style == "dusty_jazz_rap":
        return [f"{names[0]}m9", f"{names[2]}maj7", f"{names[1]}m7", f"{_PITCH_NAMES[(root + 7) % 12]}7"]
    return [f"{names[0]}m9", f"{names[1]}m9", f"{names[2]}maj7", f"{_PITCH_NAMES[(root + 9) % 12]}7"]


def _sections_for_bars(total_bars: int) -> list[dict[str, Any]]:
    if total_bars == 72:
        return [dict(section) for section in DEFAULT_SECTIONS]
    if total_bars >= 32:
        template = [("intro", 4), ("verse", 8), ("hook", 4), ("verse", 8), ("hook", 4)]
        if total_bars >= 36:
            template.append(("bridge", 4))
        template.append(("outro", 4))
    elif total_bars >= 16:
        template = [("intro", 2), ("verse", 6), ("hook", 4), ("verse", 2), ("outro", 2)]
    else:
        intro = min(2, max(1, total_bars // 4))
        outro = min(2, max(1, total_bars // 4))
        template = [("intro", intro), ("verse", max(1, total_bars - intro - outro)), ("outro", outro)]
    sections = []
    remaining = total_bars
    index = 0
    while remaining:
        kind, preferred = template[index % len(template)]
        length = min(preferred, remaining)
        sections.append({"type": kind, "bars": length})
        remaining -= length
        index += 1
    return sections


def config_from_prompt(brief: str, seed: int = 48127) -> dict[str, Any]:
    if not brief.strip():
        raise ValueError("composition brief cannot be empty")
    text = brief.strip()
    lowered = text.lower()
    if any(word in lowered for word in ("gospel", "church", "praise")):
        style = "gospel_rnb"
    elif "quiet storm" in lowered or "smooth r&b" in lowered:
        style = "quiet_storm"
    elif "jazz-rap" in lowered or "jazz rap" in lowered or "dusty" in lowered:
        style = "dusty_jazz_rap"
    elif "half-time" in lowered or "screw" in lowered or "southern" in lowered:
        style = "screw_half_time"
    else:
        style = "west_coast_smoker"

    tempo_match = re.search(r"\b(\d{2,3})\s*(?:bpm|beats per minute)\b", lowered)
    tempo = int(tempo_match.group(1)) if tempo_match else 95
    key = "D"
    scale = "natural_minor"
    key_match = _KEY_MODE.search(text)
    if key_match:
        matched_key = key_match.group(1)
        key = matched_key[0].upper() + matched_key[1:]
        mode = key_match.group(3).lower()
        scale = {"minor": "natural_minor", "major": "major"}.get(mode, mode)
        if key_match.group(2):
            scale = "natural_minor"
    bars_match = re.search(r"\b(\d{1,3})\s*bars?\b", lowered)
    total_bars = int(bars_match.group(1)) if bars_match else 32
    seed_match = re.search(r"\bseed\s*[:=]?\s*(\d+)\b", lowered)
    actual_seed = int(seed_match.group(1)) if seed_match else seed

    progression: list[str] = []
    progression_match = re.search(r"(?:progression|chords?)\s*[:=]\s*(.+?)(?:\.|;|\n|$)", text, re.I)
    if progression_match:
        progression = _CHORD_TOKEN.findall(progression_match.group(1))
        for chord in progression:
            parse_chord(chord)
    if not progression:
        progression = _default_progression(style, key)

    if not 20 <= tempo <= 300:
        raise ValueError("brief tempo must be between 20 and 300 BPM")
    if not 1 <= total_bars <= 512:
        raise ValueError("brief arrangement length must be between 1 and 512 bars")
    if style not in STYLE_PROFILES:
        raise ValueError(f"unsupported style profile selected by brief: {style}")
    title = re.sub(r"\s+", " ", text)[:48].strip(" .,:;") or "Untitled Composition"
    return {
        "project": {"title": title, "schema_version": 1, "seed": actual_seed},
        "music": {"tempo_bpm": tempo, "time_signature": [4, 4], "key": key, "scale": scale, "swing": 0.12},
        "style": {"profile": style, "harmonic_density": 0.7, "melodic_density": 0.4, "rhythmic_density": 0.5},
        "harmony": {"progression": progression, "bars_per_chord": 1, "voice_leading": True},
        "arrangement": {"sections": _sections_for_bars(total_bars)},
        "tracks": {"pad": True, "electric_piano": True, "bass": True, "lead": True},
        "export": {"format": 1, "midi_ppq": 960, "include_tempo": True, "include_markers": True},
    }
