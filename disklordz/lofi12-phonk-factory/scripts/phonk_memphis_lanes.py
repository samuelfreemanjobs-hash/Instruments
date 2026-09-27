"""Memphis artist / producer lanes — prompt tags → groove + tone bias."""

from __future__ import annotations

import re
from dataclasses import dataclass


@dataclass(frozen=True)
class MemphisLane:
    lane_id: str
    display_name: str
    aliases: tuple[str, ...]
    default_bpm: float
    kick_weights: tuple[float, ...]  # aligned to KICK_TEMPLATES in phonk_groove
    hat_density: float  # multiplier on max hats (0.5–1.2)
    cowbell_extra: int  # add to bell count
    kick_dist_threshold: float  # lower = more distorted layer (vs _rng compare)
    clap_threshold: float  # lower = more claps on snare
    swing_bias: float
    kick_pitch_delta: float = 0.0
    kick_decay_mult: float = 1.0
    snare_snap_mult: float = 1.0
    grit_delta: float = 0.0


# Template order: (0,6,8), (0,10), (0,3,6,11), (0,8,14), (0,6,8,14)
LANES: tuple[MemphisLane, ...] = (
    MemphisLane(
        "dj_paul",
        "DJ Paul",
        (r"dj\s*paul", r"\bpaul\b", r"three\s*6", r"36 mafia", r"triple\s*6"),
        84.0,
        (1.0, 0.6, 2.2, 0.9, 1.1),
        1.0,
        1,
        0.68,
        0.32,
        0.02,
        kick_pitch_delta=-2,
        grit_delta=0.08,
    ),
    MemphisLane(
        "juicy_j",
        "Juicy J",
        (r"juicy\s*j", r"\bjuicy\b"),
        86.0,
        (1.1, 0.7, 1.8, 1.0, 1.2),
        1.05,
        1,
        0.65,
        0.28,
        0.025,
        snare_snap_mult=1.05,
        grit_delta=0.06,
    ),
    MemphisLane(
        "dj_zirk",
        "DJ Zirk",
        (r"dj\s*zirk", r"\bzirk\b"),
        80.0,
        (1.2, 1.4, 0.7, 1.3, 0.6),
        0.85,
        0,
        0.78,
        0.42,
        0.0,
        kick_decay_mult=1.1,
        grit_delta=0.04,
    ),
    MemphisLane(
        "shawty_pimp",
        "Shawty Pimp",
        (r"shawty\s*pimp", r"\bshawty\b"),
        74.0,
        (0.8, 1.6, 0.6, 1.4, 0.5),
        0.65,
        0,
        0.82,
        0.48,
        0.04,
        kick_pitch_delta=-4,
        kick_decay_mult=1.2,
        snare_snap_mult=0.92,
    ),
    MemphisLane(
        "kingpin_skinny_pimp",
        "Kingpin Skinny Pimp",
        (
            r"kingpin",
            r"skinny\s*pimp",
            r"short\s*pimp",
            r"kingpin\s*skinny",
        ),
        76.0,
        (1.0, 1.5, 0.9, 1.2, 0.7),
        0.72,
        0,
        0.8,
        0.4,
        0.035,
        kick_pitch_delta=-3,
        snare_snap_mult=0.95,
    ),
    MemphisLane(
        "blackout",
        "Blackout",
        (r"blackout", r"black\s*out"),
        88.0,
        (1.3, 0.9, 1.0, 1.1, 1.0),
        0.9,
        1,
        0.55,
        0.35,
        0.01,
        grit_delta=0.12,
        snare_snap_mult=1.08,
    ),
    MemphisLane(
        "toy_wright_iii",
        "Toy Wright III",
        (r"toy\s*wright", r"wright\s*iii", r"toy\s*wright\s*iii"),
        72.0,
        (0.7, 1.7, 0.5, 1.5, 0.4),
        0.6,
        0,
        0.85,
        0.5,
        0.045,
        kick_decay_mult=1.18,
        grit_delta=0.05,
    ),
    MemphisLane(
        "apoc_crisis",
        "Apoc / crisis Memphis",
        (
            r"apoc",
            r"apocalypse",
            r"crisis",
            r"apoc\s*crisis",
            r"666",
        ),
        79.0,
        (1.1, 1.2, 0.8, 1.4, 0.8),
        0.78,
        0,
        0.62,
        0.38,
        0.03,
        grit_delta=0.15,
        snare_snap_mult=1.1,
        kick_pitch_delta=-1,
    ),
)

LANE_BY_ID = {lane.lane_id: lane for lane in LANES}


def resolve_memphis_lane(prompt: str, lane_override: str | None = None) -> MemphisLane | None:
    if lane_override:
        key = lane_override.strip().lower().replace("-", "_").replace(" ", "_")
        if key in LANE_BY_ID:
            return LANE_BY_ID[key]
        for lane in LANES:
            if key in lane.lane_id or key in lane.display_name.lower():
                return lane
        return None

    p = prompt.lower()
    matched: list[tuple[int, MemphisLane]] = []
    for lane in LANES:
        for i, pat in enumerate(lane.aliases):
            if re.search(pat, p):
                matched.append((i, lane))
                break
    if not matched:
        return None
    matched.sort(key=lambda x: x[0])
    return matched[0][1]


def list_lane_ids() -> list[str]:
    return [lane.lane_id for lane in LANES]
