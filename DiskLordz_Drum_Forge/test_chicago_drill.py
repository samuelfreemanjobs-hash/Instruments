"""Contract tests for DiskLordz Chicago Drill 16-Pad & Groove Factory (v2)."""
from pathlib import Path
import shutil

import numpy as np

import chicago_drill_lab as drill
import forge_core_v2 as core


def test_straight_and_octave_glide_808_pads():
    # Pad 1 (F1 43.65 Hz) & Pad 2 (G1 49.00 Hz) hold straight root pitch
    for idx in (0, 1):
        preset = drill.CHICAGO_16_PAD_MATRIX[idx]
        sig = drill.render_drill_pad(preset, sr=48000).mean(axis=1)
        detected, conf = core.estimate_808_pitch(sig, 48000, preset.freq_or_pitch)
        assert detected is not None and abs(12.0 * np.log2(detected / preset.freq_or_pitch)) < 0.45 and conf >= 0.5

    # Pad 3 (+12 st @ 0.50 from G1 49 Hz -> G2 98 Hz) glides up an octave in the second half
    octave_pad = drill.render_drill_pad(drill.CHICAGO_16_PAD_MATRIX[2], sr=48000).mean(axis=1)
    early = octave_pad[int(0.22 * 48000) : int(0.45 * 48000)]
    late = octave_pad[int(0.95 * 48000) : int(1.30 * 48000)]
    det_early, _ = core.estimate_808_pitch(np.pad(early, (int(0.24 * 48000), 0)), 48000, 48.9994)
    det_late, _ = core.estimate_808_pitch(np.pad(late, (int(0.24 * 48000), 0)), 48000, 97.9989)
    assert det_early is not None and det_late is not None
    assert 10.5 <= 12.0 * np.log2(det_late / det_early) <= 13.5


def test_downbeat_crash_has_stereo_shimmer_and_no_tail_click():
    crash = drill.render_drill_pad(drill.CHICAGO_16_PAD_MATRIX[15], sr=48000)
    assert crash.ndim == 2 and not np.allclose(crash[:, 0], crash[:, 1])
    assert abs(float(crash[-1, 0])) < 1e-4  # smooth fade, not truncated at (decaySec + 0.15)*sr
    mono = crash.mean(axis=1)
    mag = np.abs(np.fft.rfft(mono)) ** 2
    freqs = np.fft.rfftfreq(len(mono), 1.0 / 48000.0)
    cymbal_band = float(mag[(freqs >= 3600.0) & (freqs <= 14000.0)].sum())
    low_mud = float(mag[freqs < 1500.0].sum())
    assert cymbal_band > low_mud * 20.0


def test_chicago_drill_16_pad_matrix_and_midi_export():
    out = Path("C:/dev/Instruments/DiskLordz_Chicago_Drill_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = drill.export_chicago_drill_kit(out, sr=48000, seed=312808, bpm=134.0, make_zip=True)
    assert manifest["count"] == 16
    assert (out / "01_808_Chop_ElChopo_F1.wav").is_file()
    assert (out / "03_808_Durk_OctaveSlide_G1.wav").is_file()
    assert (out / "16_Crash_Downbeat_Drill.wav").is_file()
    assert (out / "17_SouthSide_Drill_134BPM.mid").is_file()
    assert (out / "DiskLordz_Chicago_Drill_16Pad_Pack.zip").is_file()


if __name__ == "__main__":
    test_straight_and_octave_glide_808_pads()
    test_downbeat_crash_has_stereo_shimmer_and_no_tail_click()
    test_chicago_drill_16_pad_matrix_and_midi_export()
    print("Chicago Drill v2 16-pad tests passed")
