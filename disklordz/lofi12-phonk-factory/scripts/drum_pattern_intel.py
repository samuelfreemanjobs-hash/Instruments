"""Pattern / MIDI intelligence — Memphis vs Phonk grammars (Phase 1)."""

from __future__ import annotations

import hashlib
import math
from dataclasses import dataclass

from factory_modes import FactoryModeProfile

STEPS = 16
GM = {
    "kick": 36,
    "snare": 38,
    "clap": 39,
    "hat": 42,
    "hat_open": 46,
    "cowbell": 56,
    "rim": 37,
    "tom": 45,
    "crash": 49,
}

NOTE_NAMES = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")


def midi_to_key(note: int) -> str:
    return f"{NOTE_NAMES[note % 12]}{note // 12 - 1}"


def pick_bpm(mode: FactoryModeProfile, seed: int) -> float:
    span = mode.bpm_hi - mode.bpm_lo
    frac = (seed % 1000) / 1000.0
    return mode.bpm_lo + span * frac


def pick_swing(mode: FactoryModeProfile, seed: int) -> float:
    span = mode.swing_hi - mode.swing_lo
    frac = ((seed >> 8) % 1000) / 1000.0
    base = mode.swing_lo + span * frac
    density_bias = (mode.fusion.density_swing - 0.5) * 0.08
    return min(0.64, max(0.50, base + density_bias))


@dataclass
class PatternNote:
    step: int
    instrument: str
    velocity: int
    micro_ms: float = 0.0


def _memphis_bar(bar: int, *, swing: float, seed: int, density_swing: float) -> list[PatternNote]:
    notes: list[PatternNote] = []
    kicks = (0, 6, 8) if bar % 2 == 0 else (0, 10)
    if (seed + bar) % 5 == 0:
        kicks = kicks + (3,)  # ghost kick
    for st in kicks:
        vel = 110 if st == 0 else 72
        delay = 6.0 if st != 0 else 0.0
        notes.append(PatternNote(st, "kick", vel, delay))
    for st in (4, 12):
        notes.append(PatternNote(st, "snare", 115))
    for st in (2, 7, 9, 13):
        notes.append(PatternNote(st, "cowbell", 88))
    step = 2 if density_swing < 0.55 else 1
    hat_steps = list(range(0, 16, step))
    if bar == 1 and (seed % 3) == 0:
        hat_steps.extend([14, 15])  # pitch-drop roll proxy (denser at end)
    for i, st in enumerate(hat_steps):
        offbeat = st % 4 in (1, 3)
        micro = (swing - 0.5) * 18.0 if offbeat else 0.0
        notes.append(PatternNote(st, "hat", 70 + (i % 5) * 3, micro))
    return notes


def _late(base_ms: float, seed: int, step: int) -> float:
    jitter = ((seed + step * 17) % 13) / 13.0 * 5.0
    return base_ms + jitter


def _occult_tape_bar(bar: int, *, seed: int) -> list[PatternNote]:
    """Slow dragging groove — notes humanized late behind the grid."""
    notes: list[PatternNote] = []
    lag = 9.0
    kicks = (0, 8) if bar % 2 == 0 else (0, 6, 14)
    for st in kicks:
        vel = 108 if st == 0 else 78
        notes.append(PatternNote(st, "kick", vel, _late(lag, seed, st)))
    for st in (4, 12):
        notes.append(PatternNote(st, "snare", 112, _late(lag + 2, seed, st)))
        notes.append(PatternNote(st, "rim", 94, _late(lag + 5, seed, st + 100)))
    # detuned 808-style cowbell bounce
    for st in (0, 3, 6, 9, 12, 15):
        vel = 96 if st % 6 == 0 else 82
        notes.append(PatternNote(st, "cowbell", vel, _late(lag + 4, seed, st + 40)))
    # sparse 16th hat rolls (brief clusters only)
    roll_start = 13 if (seed + bar) % 2 == 0 else 6
    for st in (roll_start, roll_start + 1):
        if st < 16:
            notes.append(PatternNote(st, "hat", 68 + st, _late(lag + 3, seed, st + 200)))
    if bar == 1 and (seed % 3) == 0:
        notes.append(PatternNote(7, "hat", 62, _late(lag + 6, seed, 7)))
    return notes


def _mpc_swing_ms(step: int, swing: float, bpm: float) -> float:
    """Delay even 16th-note positions (steps 1,3,5,…) MPC-style."""
    if step % 2 == 1:
        sixteenth_ms = (60_000.0 / bpm) / 4.0
        return (swing - 0.5) * sixteenth_ms * 0.9
    return 0.0


def _atl_early_trap_bar(
    bar: int, *, swing: float, seed: int, hard: bool, bpm: float = 144.0
) -> list[PatternNote]:
    notes: list[PatternNote] = []
    kicks = (0,) if bar % 2 == 0 else (0, 8)
    for st in kicks:
        notes.append(PatternNote(st, "kick", 115 if st == 0 else 95))
    notes.append(PatternNote(8, "snare", 112 if not hard else 120))
    notes.append(PatternNote(12, "clap", 98))
    for st in range(0, 16, 2):
        micro = _mpc_swing_ms(st + 1, swing, bpm) if st + 1 < 16 else 0.0
        notes.append(PatternNote(st, "hat", 62 + (st % 4) * 4, micro))
    for st in range(1, 16, 2):
        notes.append(PatternNote(st, "hat", 58, _mpc_swing_ms(st, swing, bpm)))
    if (seed + bar) % 2 == 0:
        for k in range(4):
            vel = int(52 + 48 * (1.0 - math.exp(-k * 0.65)))
            notes.append(PatternNote(14, "hat", vel, k * 6.5))
        for k in range(3):
            vel = int(48 + 40 * (1.0 - math.exp(-k * 0.55)))
            notes.append(PatternNote(15, "hat", vel, k * 4.2))
    if bar == 1 or (hard and bar == 0):
        for st, vel in ((10, 90), (11, 85), (12, 88)):
            notes.append(PatternNote(st, "tom", vel))
    if bar == 0 and (seed % 4) == 0:
        notes.append(PatternNote(0, "crash", 100))
    return notes


def _phonk_bar(bar: int, *, swing: float, seed: int, black_cassette: bool) -> list[PatternNote]:
    notes: list[PatternNote] = []
    kick_steps = (0, 4, 8, 10, 12) if bar % 2 == 0 else (0, 3, 6, 8, 12)
    for st in kick_steps:
        notes.append(PatternNote(st, "kick", 120 if st == 0 else 100))
    for st in (4, 12):
        notes.append(PatternNote(st, "snare", 118))
        if black_cassette:
            notes.append(PatternNote(st, "rim", 95, 2.0))
    # continuous cowbell gallop
    for st in (0, 2, 4, 6, 8, 10, 12, 14):
        notes.append(PatternNote(st, "cowbell", 92 if st % 4 == 0 else 78))
    hat_div = 1 if black_cassette else 2
    for st in range(0, 16, hat_div):
        offbeat = st % 2 == 1
        micro = (swing - 0.5) * 14.0 if offbeat else 0.0
        vel = 65 + (st % 4) * 5
        if black_cassette and bar == 1 and st >= 12:
            vel += 15
        notes.append(PatternNote(st, "hat", min(127, vel), micro))
    return notes


def build_pattern(
    mode: FactoryModeProfile,
    *,
    bars: int,
    seed: int,
) -> tuple[list[PatternNote], float, float, str]:
    bpm = pick_bpm(mode, seed)
    swing = pick_swing(mode, seed)
    key_note = 36 + (seed % 12)
    key = midi_to_key(key_note)
    black = mode.mode_id == "black_cassette"
    notes: list[PatternNote] = []
    for bar in range(bars):
        if mode.mode_id == "occult_tape":
            notes.extend(_occult_tape_bar(bar, seed=seed + bar))
        elif mode.atl_trap:
            notes.extend(
                _atl_early_trap_bar(
                    bar,
                    swing=swing,
                    seed=seed + bar,
                    hard=mode.mode_id == "trap_hard_2006",
                    bpm=bpm,
                )
            )
        elif mode.phonk_pattern:
            notes.extend(_phonk_bar(bar, swing=swing, seed=seed, black_cassette=black))
        else:
            notes.extend(
                _memphis_bar(bar, swing=swing, seed=seed, density_swing=mode.fusion.density_swing)
            )
    return notes, bpm, swing, key


def pattern_provenance(mode_id: str, notes: list[PatternNote], bpm: float, seed: int) -> str:
    blob = f"{mode_id}|{seed}|{bpm:.2f}|" + "|".join(f"{n.step}:{n.instrument}" for n in notes)
    return hashlib.sha256(blob.encode()).hexdigest()[:16]
