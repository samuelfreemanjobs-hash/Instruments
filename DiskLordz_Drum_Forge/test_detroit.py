"""Detroit lab stays its own engine: short 808s, dry rims, and bounce MIDI."""
from pathlib import Path

import numpy as np

import detroit_lab as lab
import forge_core_v2 as core


def test_detroit_808_is_shorter_than_atlanta():
    detroit, root = lab.gen_808(44100, np.random.default_rng(4), 24)
    atlanta, _root = core.gen_808(44100, np.random.default_rng(4), core.PRESETS["atl_trap"], 24)
    assert len(detroit) < len(atlanta)
    assert len(detroit) < int(44100 * 0.75)
    mastered = core.master(detroit, 44100, -1.0)
    detected, confidence = core.estimate_808_pitch(mastered, 44100, root)
    assert detected is not None
    assert abs(12 * np.log2(detected / root)) < 0.5
    assert confidence > 0.48


def test_rim_is_dry_and_hats_are_finite():
    rim, _root = lab.gen_rim(44100, np.random.default_rng(9))
    attack = np.max(np.abs(rim[: int(0.015 * 44100)]))
    tail = np.max(np.abs(rim[int(0.06 * 44100):]))
    assert tail < attack * 0.3
    hat, _root = lab.gen_hat(48000, np.random.default_rng(2), False)
    assert np.all(np.isfinite(hat))
    assert len(hat) < int(48000 * 0.08)


def test_bounce_midi_has_kick_hat_and_808():
    events = lab.bounce_events(np.random.default_rng(11), 32, 0.16, 24)
    path = Path("detroit-bounce-test.mid")
    lab.write_midi(path, events, 142)
    try:
        notes = lab.read_note_ons(path)
    finally:
        path.unlink(missing_ok=True)
    pitches = {note for note, _channel in notes}
    channels = {channel for _note, channel in notes}
    assert 36 in pitches and 42 in pitches and 24 in pitches
    assert 0 in channels and 9 in channels


def test_two_grooves_are_not_the_same_file():
    first = lab.bounce_events(np.random.default_rng(1), 32, 0.1, 24)
    second = lab.bounce_events(np.random.default_rng(2), 64, 0.24, 29)
    assert first != second


if __name__ == "__main__":
    test_detroit_808_is_shorter_than_atlanta()
    test_rim_is_dry_and_hats_are_finite()
    test_bounce_midi_has_kick_hat_and_808()
    test_two_grooves_are_not_the_same_file()
    print("Detroit tests passed")
