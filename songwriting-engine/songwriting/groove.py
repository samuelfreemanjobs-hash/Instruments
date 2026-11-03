"""Groove profiles and the shared groove clock (see docs/GROOVE.md).

Generators work in grid beats. ``GrooveClock.place`` turns a grid beat into the performed beat for
a role, so every track shares one swing, accent map, pocket, section feel, and humanization.
"""

from __future__ import annotations

import random
from dataclasses import asdict, dataclass, field
from typing import Any

from .models import Section

SUBDIVISIONS = {
    "quarter": 1.0,
    "eighth": 0.5,
    "sixteenth": 0.25,
    "thirty_second": 0.125,
    "eighth_triplet": 1 / 3,
    "sixteenth_triplet": 1 / 6,
}
SWING_SUBDIVISIONS = ("eighth", "sixteenth")
ROLES = ("drums", "bass", "pad", "electric_piano", "lead")
DRUM_VOICES = ("kick", "snare", "clap", "rim", "hat_closed", "hat_open", "perc", "tom", "crash")


def _eighths(beats: int) -> tuple[float, ...]:
    return tuple(i * 0.5 for i in range(beats * 2))


def _sixteenths(beats: int) -> tuple[float, ...]:
    return tuple(i * 0.25 for i in range(beats * 4))


def _quarters(beats: int) -> tuple[float, ...]:
    return tuple(float(i) for i in range(beats))


@dataclass(frozen=True, slots=True)
class Humanize:
    timing: float
    velocity: int
    duration: float


HUMANIZE_PRESETS = {
    "tight": Humanize(0.0, 2, 0.0),
    "subtle": Humanize(0.008, 4, 0.03),
    "natural": Humanize(0.015, 7, 0.06),
    "loose": Humanize(0.025, 10, 0.10),
    "live": Humanize(0.04, 14, 0.15),
}
ROLE_INTENSITY = {"drums": 1.0, "bass": 0.8, "pad": 0.4, "electric_piano": 0.9, "lead": 1.0}


@dataclass(frozen=True, slots=True)
class VelocityDynamics:
    base: int
    spread: int
    accent: int
    ghost: int


@dataclass(frozen=True, slots=True)
class DrumPattern:
    """Grid beats within a ``bars``-long pattern (4/4); voices absent from a tuple never play."""

    bars: int
    kick: tuple[float, ...]
    snare: tuple[float, ...]
    hat_closed: tuple[float, ...]
    hat_open: tuple[float, ...] = ()
    clap: tuple[float, ...] = ()
    rim: tuple[float, ...] = ()
    perc: tuple[float, ...] = ()
    ghost_snare: tuple[float, ...] = ()
    hat_probability: float = 0.9
    perc_note: int = 82
    tom_fills: bool = False


@dataclass(frozen=True, slots=True)
class BassRhythm:
    follow_kick: float
    anticipation: float
    passing_notes: bool
    max_per_bar: int
    sustain: float
    octave_pops: float = 0.0


@dataclass(frozen=True, slots=True)
class ChordRhythm:
    pad_anticipation: float
    pad_delay: float
    ep_stabs: tuple[tuple[float, float], ...]
    ep_length: float


@dataclass(frozen=True, slots=True)
class MelodyRhythm:
    cells: tuple[tuple[tuple[float, float], ...], ...]
    phrase_bars: int = 4


@dataclass(frozen=True, slots=True)
class GrooveProfile:
    id: str
    name: str
    description: str
    tempo_range: tuple[int, int]
    meter: tuple[int, int]
    subdivision: str
    swing_percent: float
    swing_subdivision: str
    timing: tuple[tuple[str, float], ...]
    velocity: VelocityDynamics
    accents: tuple[float, ...]
    density: float
    humanize: str
    half_time: bool
    drums: DrumPattern
    bass: BassRhythm
    chords: ChordRhythm
    melody: MelodyRhythm

    def offset(self, role: str) -> float:
        return dict(self.timing).get(role, dict(self.timing).get(role.split(":")[0], 0.0))

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass(frozen=True, slots=True)
class SectionGroove:
    drum_voices: frozenset[str]
    density: float
    push: float
    intensity: float
    muted_roles: tuple[str, ...] = ()


_ALL_DRUMS = frozenset(DRUM_VOICES) - {"clap"}
SECTION_GROOVES = {
    "intro": SectionGroove(frozenset({"kick", "hat_closed", "rim", "crash"}), 0.6, 0.0, 0.8, ("lead",)),
    "verse": SectionGroove(_ALL_DRUMS, 0.85, 0.0, 1.0),
    "hook": SectionGroove(frozenset(DRUM_VOICES), 1.0, -0.004, 1.1),
    "bridge": SectionGroove(frozenset({"kick", "rim", "hat_open", "perc", "tom", "crash"}), 0.65, 0.004, 0.9),
    "outro": SectionGroove(frozenset({"kick", "hat_closed", "crash"}), 0.5, 0.006, 0.8, ("lead",)),
}
DEFAULT_SECTION_GROOVE = SECTION_GROOVES["verse"]

_SMOKER_CELLS = (
    ((0.5, 0.5), (1.5, 1.0), (3.0, 0.5), (3.5, 1.0), (5.5, 0.5), (6.0, 1.5)),
    ((1.0, 0.75), (2.0, 0.5), (2.5, 1.0), (4.5, 1.0), (6.5, 1.0)),
)
GROOVE_PROFILES: dict[str, GrooveProfile] = {
    "west_coast_smoker": GrooveProfile(
        id="west_coast_smoker",
        name="West Coast Smoker",
        description="Laid-back pocket: snare and keys sit behind the beat, spacious 8th hats, kick/snare call-and-response, bass on the kick without doubling every hit.",
        tempo_range=(82, 100),
        meter=(4, 4),
        subdivision="sixteenth",
        swing_percent=58.0,
        swing_subdivision="sixteenth",
        timing=(("drums:kick", 0.0), ("drums:snare", 0.035), ("drums:clap", 0.035), ("drums:hat_closed", 0.015),
                ("drums:hat_open", 0.015), ("drums", 0.01), ("bass", 0.025), ("pad", 0.01),
                ("electric_piano", 0.04), ("lead", 0.05)),
        velocity=VelocityDynamics(base=92, spread=8, accent=14, ghost=34),
        accents=(1.0, 3.0),
        density=0.6,
        humanize="natural",
        half_time=False,
        drums=DrumPattern(
            bars=2, kick=(0.0, 2.5, 4.0, 5.75, 6.5), snare=(1.0, 3.0, 5.0, 7.0), hat_closed=_eighths(8),
            hat_open=(7.5,), clap=(3.0, 7.0), rim=(7.75,), ghost_snare=(2.75, 6.25), hat_probability=0.85,
        ),
        bass=BassRhythm(follow_kick=0.75, anticipation=0.0, passing_notes=True, max_per_bar=4, sustain=0.85),
        chords=ChordRhythm(pad_anticipation=0.0, pad_delay=0.0,
                           ep_stabs=((0.0, 0.7), (1.5, 0.5), (2.5, 0.6), (3.75, 0.3)), ep_length=0.4),
        melody=MelodyRhythm(cells=_SMOKER_CELLS),
    ),
    "screw_half_time": GrooveProfile(
        id="screw_half_time",
        name="DJ Screw / Half-Time",
        description="Half-time pulse: one snare per bar on beat 3, quarter hats, long sustained bass and chords, everything dragged late.",
        tempo_range=(60, 160),
        meter=(4, 4),
        subdivision="eighth",
        swing_percent=54.0,
        swing_subdivision="eighth",
        timing=(("drums:snare", 0.05), ("drums", 0.03), ("bass", 0.04), ("pad", 0.02),
                ("electric_piano", 0.05), ("lead", 0.06)),
        velocity=VelocityDynamics(base=88, spread=6, accent=12, ghost=30),
        accents=(2.0,),
        density=0.35,
        humanize="loose",
        half_time=True,
        drums=DrumPattern(
            bars=2, kick=(0.0, 3.5, 4.0, 5.5), snare=(2.0, 6.0), hat_closed=_quarters(8), hat_open=(7.0,),
            clap=(2.0, 6.0), hat_probability=0.9,
        ),
        bass=BassRhythm(follow_kick=0.35, anticipation=0.0, passing_notes=False, max_per_bar=2, sustain=0.97),
        chords=ChordRhythm(pad_anticipation=0.0, pad_delay=0.0, ep_stabs=((0.0, 0.9), (2.5, 0.4)), ep_length=1.5),
        melody=MelodyRhythm(cells=(((1.0, 1.5), (3.0, 1.0), (5.0, 2.0)), ((0.5, 1.0), (2.0, 2.0), (6.0, 1.5))), phrase_bars=4),
    ),
    "quiet_storm": GrooveProfile(
        id="quiet_storm",
        name="90s R&B / Quiet Storm",
        description="Smooth 16th hats at low velocity, soft backbeat, gentle syncopation; pads anticipate changes by an 8th and bass ties into the next root.",
        tempo_range=(60, 96),
        meter=(4, 4),
        subdivision="sixteenth",
        swing_percent=54.0,
        swing_subdivision="sixteenth",
        timing=(("drums:snare", 0.015), ("drums", 0.005), ("bass", 0.01), ("pad", 0.0),
                ("electric_piano", 0.02), ("lead", 0.03)),
        velocity=VelocityDynamics(base=78, spread=5, accent=8, ghost=28),
        accents=(1.0, 3.0),
        density=0.5,
        humanize="subtle",
        half_time=False,
        drums=DrumPattern(
            bars=1, kick=(0.0, 2.5), snare=(1.0, 3.0), hat_closed=_sixteenths(4), rim=(3.75,), perc=(1.5, 3.5),
            ghost_snare=(2.75,), hat_probability=0.7, perc_note=82,
        ),
        bass=BassRhythm(follow_kick=0.8, anticipation=0.5, passing_notes=True, max_per_bar=3, sustain=0.9),
        chords=ChordRhythm(pad_anticipation=0.5, pad_delay=0.0,
                           ep_stabs=((0.0, 0.8), (1.5, 0.4), (2.0, 0.5), (3.5, 0.4)), ep_length=0.9),
        melody=MelodyRhythm(cells=(((0.0, 1.0), (1.5, 0.5), (2.0, 1.5), (4.5, 0.5), (5.0, 2.0)),
                                   ((0.5, 0.5), (1.0, 1.0), (2.5, 1.0), (4.0, 1.0), (5.5, 1.5)))),
    ),
    "dusty_jazz_rap": GrooveProfile(
        id="dusty_jazz_rap",
        name="Dusty Jazz-Rap",
        description="Triplet-swung 8ths, syncopated kicks and ghost snares, loose humanization, busy-then-sparse comping.",
        tempo_range=(80, 98),
        meter=(4, 4),
        subdivision="eighth",
        swing_percent=66.0,
        swing_subdivision="eighth",
        timing=(("drums:kick", -0.01), ("drums:snare", 0.03), ("drums:hat_closed", 0.02), ("drums", 0.01),
                ("bass", 0.02), ("pad", 0.01), ("electric_piano", 0.035), ("lead", 0.04)),
        velocity=VelocityDynamics(base=86, spread=11, accent=12, ghost=32),
        accents=(1.0, 3.0),
        density=0.55,
        humanize="loose",
        half_time=False,
        drums=DrumPattern(
            bars=2, kick=(0.0, 1.75, 2.5, 4.0, 5.5, 6.75), snare=(1.0, 3.0, 5.0, 7.0), hat_closed=_eighths(8),
            hat_open=(3.5,), rim=(2.75, 6.25), ghost_snare=(0.75, 2.25, 4.75, 6.25), hat_probability=0.8,
        ),
        bass=BassRhythm(follow_kick=0.7, anticipation=0.2, passing_notes=True, max_per_bar=4, sustain=0.7),
        chords=ChordRhythm(pad_anticipation=0.0, pad_delay=0.25,
                           ep_stabs=((0.0, 0.5), (0.75, 0.4), (1.5, 0.5), (2.25, 0.4), (3.0, 0.4), (3.5, 0.5)),
                           ep_length=0.3),
        melody=MelodyRhythm(cells=(((0.5, 0.5), (1.0, 0.5), (2.0, 0.5), (2.5, 1.0), (4.5, 0.5), (5.0, 0.5), (6.0, 1.0)),
                                   ((0.0, 0.5), (1.5, 0.5), (3.0, 1.0), (5.5, 0.5), (6.5, 1.0)))),
    ),
    "boom_bap": GrooveProfile(
        id="boom_bap",
        name="Classic Boom Bap",
        description="Hard kick/snare, 16th hats with 8th accents, MPC 62 % 16th swing, snare-and-tom fills into new sections.",
        tempo_range=(84, 100),
        meter=(4, 4),
        subdivision="sixteenth",
        swing_percent=62.0,
        swing_subdivision="sixteenth",
        timing=(("drums:snare", 0.02), ("drums", 0.0), ("bass", 0.01), ("pad", 0.0),
                ("electric_piano", 0.015), ("lead", 0.02)),
        velocity=VelocityDynamics(base=100, spread=7, accent=18, ghost=40),
        accents=(1.0, 3.0),
        density=0.7,
        humanize="subtle",
        half_time=False,
        drums=DrumPattern(
            bars=2, kick=(0.0, 1.75, 2.5, 4.0, 4.5, 6.25), snare=(1.0, 3.0, 5.0, 7.0), hat_closed=_sixteenths(8),
            hat_open=(7.5,), ghost_snare=(3.75, 5.75), hat_probability=0.9, tom_fills=True,
        ),
        bass=BassRhythm(follow_kick=0.9, anticipation=0.0, passing_notes=False, max_per_bar=4, sustain=0.6),
        chords=ChordRhythm(pad_anticipation=0.0, pad_delay=0.0,
                           ep_stabs=((0.0, 0.6), (1.75, 0.4), (2.5, 0.5)), ep_length=0.35),
        melody=MelodyRhythm(cells=(((0.0, 0.5), (0.5, 0.5), (1.5, 0.5), (2.0, 1.0), (4.5, 0.5), (5.0, 0.5), (6.0, 1.5)),)),
    ),
    "funk": GrooveProfile(
        id="funk",
        name="Funk-Inspired",
        description="Syncopated 16th bass with octave pops locked to the kick, ghost-note snare, chicken-scratch keys, conga accents.",
        tempo_range=(92, 120),
        meter=(4, 4),
        subdivision="sixteenth",
        swing_percent=54.0,
        swing_subdivision="sixteenth",
        timing=(("drums:snare", 0.005), ("drums", 0.0), ("bass", -0.005), ("pad", 0.0),
                ("electric_piano", 0.0), ("lead", 0.01)),
        velocity=VelocityDynamics(base=96, spread=9, accent=16, ghost=38),
        accents=(1.0, 3.0),
        density=0.85,
        humanize="natural",
        half_time=False,
        drums=DrumPattern(
            bars=1, kick=(0.0, 0.75, 2.5), snare=(1.0, 3.0), hat_closed=_sixteenths(4), hat_open=(1.5,),
            perc=(0.5, 1.25, 2.75, 3.25), ghost_snare=(1.75, 2.25, 3.75), hat_probability=0.95,
            perc_note=63, tom_fills=True,
        ),
        bass=BassRhythm(follow_kick=0.95, anticipation=0.3, passing_notes=True, max_per_bar=6, sustain=0.5, octave_pops=0.35),
        chords=ChordRhythm(pad_anticipation=0.0, pad_delay=0.0,
                           ep_stabs=((0.25, 0.6), (0.75, 0.6), (1.5, 0.6), (2.25, 0.6), (2.75, 0.5), (3.5, 0.6)),
                           ep_length=0.18),
        melody=MelodyRhythm(cells=(((0.0, 0.25), (0.25, 0.5), (1.0, 0.5), (1.75, 0.5), (2.5, 1.0), (4.0, 0.5), (4.75, 0.5), (5.5, 1.5)),)),
    ),
    "gospel_rnb": GrooveProfile(
        id="gospel_rnb",
        name="90s Gospel R&B",
        description="Shuffled 16ths, clap layered on the backbeat, tambourine on 2 and 4, busy comping that answers the lead.",
        tempo_range=(64, 100),
        meter=(4, 4),
        subdivision="sixteenth",
        swing_percent=60.0,
        swing_subdivision="sixteenth",
        timing=(("drums:snare", 0.02), ("drums", 0.005), ("bass", 0.01), ("pad", 0.0),
                ("electric_piano", 0.02), ("lead", 0.03)),
        velocity=VelocityDynamics(base=90, spread=8, accent=14, ghost=34),
        accents=(1.0, 3.0),
        density=0.75,
        humanize="natural",
        half_time=False,
        drums=DrumPattern(
            bars=1, kick=(0.0, 1.5, 2.5), snare=(1.0, 3.0), hat_closed=_sixteenths(4), hat_open=(3.5,),
            clap=(1.0, 3.0), perc=(1.0, 3.0), ghost_snare=(2.75,), hat_probability=0.85, perc_note=54,
        ),
        bass=BassRhythm(follow_kick=0.8, anticipation=0.3, passing_notes=True, max_per_bar=4, sustain=0.8),
        chords=ChordRhythm(pad_anticipation=0.25, pad_delay=0.0,
                           ep_stabs=((0.0, 0.8), (0.75, 0.5), (1.5, 0.6), (2.0, 0.5), (2.75, 0.5), (3.5, 0.6)),
                           ep_length=0.45),
        melody=MelodyRhythm(cells=(((0.0, 0.25), (0.25, 0.25), (0.5, 0.5), (1.0, 1.0), (2.5, 0.5), (3.0, 1.0), (4.5, 0.5), (5.0, 0.5), (5.5, 1.5)),)),
    ),
}


@dataclass(frozen=True, slots=True)
class GrooveSettings:
    """What a project records about its groove (stored on ``Project.groove``)."""

    profile: str = ""
    humanize: str = ""
    swing_percent: float = 50.0
    quantize_strength: float = 0.0


def validate_groove_config(groove: Any, style: str) -> GrooveSettings:
    groove = {} if groove is None else groove
    if not isinstance(groove, dict):
        raise ValueError("groove must be an object")
    unknown = set(groove) - {"profile", "humanize", "swing_percent", "quantize_strength"}
    if unknown:
        raise ValueError(f"unsupported groove options: {sorted(unknown)}")
    profile_id = groove.get("profile") or style
    if profile_id not in GROOVE_PROFILES:
        raise ValueError(f"unsupported groove profile: {profile_id!r}")
    profile = GROOVE_PROFILES[profile_id]
    humanize = groove.get("humanize") or profile.humanize
    if humanize not in HUMANIZE_PRESETS:
        raise ValueError(f"unsupported humanize preset: {humanize!r}")
    swing = groove.get("swing_percent", profile.swing_percent)
    if not isinstance(swing, (int, float)) or not 50 <= swing <= 75:
        raise ValueError("groove.swing_percent must be between 50 and 75")
    strength = groove.get("quantize_strength", 0.0)
    if not isinstance(strength, (int, float)) or not 0 <= strength <= 1:
        raise ValueError("groove.quantize_strength must be between 0 and 1")
    return GrooveSettings(profile_id, humanize, float(swing), float(strength))


def section_groove(section: Section) -> SectionGroove:
    base = SECTION_GROOVES.get(section.kind, DEFAULT_SECTION_GROOVE)
    return SectionGroove(
        base.drum_voices,
        base.density * section.variation.density,
        base.push,
        base.intensity,
        tuple(sorted(set(base.muted_roles) | set(section.variation.muted_roles))),
    )


@dataclass
class GrooveClock:
    """Shared timing context for every track of one project."""

    profile: GrooveProfile
    settings: GrooveSettings
    seed: int
    beats_per_bar: float
    sections: tuple[Section, ...]
    _section_cache: dict[int, tuple[Section, SectionGroove]] = field(default_factory=dict)

    @property
    def total_beats(self) -> float:
        return sum(s.bars for s in self.sections) * self.beats_per_bar

    @property
    def humanize(self) -> Humanize:
        return HUMANIZE_PRESETS[self.settings.humanize]

    def section_at(self, beat: float) -> tuple[Section, SectionGroove]:
        bar = int(beat // self.beats_per_bar)
        if bar not in self._section_cache:
            section = next(
                (s for s in self.sections if s.start_bar <= bar < s.start_bar + s.bars), self.sections[-1]
            )
            self._section_cache[bar] = (section, section_groove(section))
        return self._section_cache[bar]

    def _noise(self, *key: object) -> float:
        return random.Random(":".join(str(k) for k in (self.seed, *key))).uniform(-1.0, 1.0)

    def swing(self, beat: float) -> float:
        step = SUBDIVISIONS[self.profile.swing_subdivision]
        pair = 2 * step
        pair_start = (beat // pair) * pair
        if abs((beat - pair_start) - step) < 1e-9:
            return pair_start + pair * self.settings.swing_percent / 100
        return beat

    def deviation(self, beat: float, role: str) -> float:
        _, groove = self.section_at(beat)
        slot = round(beat / 0.25)
        human = self.humanize.timing * ROLE_INTENSITY.get(role.split(":")[0], 1.0) * groove.intensity
        human *= 0.7 * self._noise("slot", slot) + 0.3 * self._noise(role, slot)
        raw = self.profile.offset(role) + groove.push + human
        limit = min(0.4 * SUBDIVISIONS[self.profile.subdivision], 0.08)
        return max(-limit, min(limit, raw))

    def place(self, beat: float, role: str) -> float:
        step = SUBDIVISIONS[self.profile.subdivision]
        strength = self.settings.quantize_strength
        pulled = beat + (round(beat / step) * step - beat) * strength
        performed = self.swing(pulled) + self.deviation(pulled, role) * (1.0 - strength)
        return max(0.0, min(self.total_beats - 1 / 960, performed))

    def velocity(self, base: int, beat: float, role: str, *, accent: bool = False, ghost: bool = False) -> int:
        _, groove = self.section_at(beat)
        dynamics = self.profile.velocity
        if ghost:
            value = float(dynamics.ghost)
        else:
            value = base * groove.intensity
            in_bar = beat % self.beats_per_bar
            if accent or any(abs(in_bar - a) < 1e-9 for a in self.profile.accents):
                value += dynamics.accent
        spread = max(dynamics.spread, self.humanize.velocity) * ROLE_INTENSITY.get(role.split(":")[0], 1.0)
        value += spread * (0.6 * self._noise("vel", round(beat / 0.25)) + 0.4 * self._noise("vel", role, beat))
        return max(1, min(127, round(value)))

    def duration(self, beats: float, beat: float, role: str) -> float:
        variation = self.humanize.duration * ROLE_INTENSITY.get(role.split(":")[0], 1.0)
        return max(1 / 96, beats * (1.0 + variation * self._noise("dur", role, beat)))
