"""Contract tests for DiskLordz Louisiana v2 (Baton Rouge, BooBass, NOLA Formant Chants & Triggerman)."""
from pathlib import Path
import shutil

import numpy as np

import forge_core_v2 as core
import louisiana_bounce_lab as la


def test_baton_rouge_808_and_swept_svf_boo_bass():
    sig_808 = la.render_louisiana_pad(la.LOUISIANA_16_PAD_MATRIX[0], sr=48000).mean(axis=1)
    detected, conf = core.estimate_808_pitch(sig_808, 48000, 43.6535)
    assert detected is not None and abs(12.0 * np.log2(detected / 43.6535)) < 0.45 and conf >= 0.5

    boo = la.render_louisiana_pad(la.LOUISIANA_16_PAD_MATRIX[2], sr=48000).mean(axis=1)
    assert np.all(np.isfinite(boo))
    # Swept 1600 -> 240 Hz SVF makes attack brighter than tail
    early_mag = np.abs(np.fft.rfft(boo[: int(0.05 * 48000)]))
    late_mag = np.abs(np.fft.rfft(boo[int(0.15 * 48000) : int(0.30 * 48000)]))
    f_early = np.fft.rfftfreq(int(0.05 * 48000), 1.0 / 48000.0)
    f_late = np.fft.rfftfreq(int(0.15 * 48000), 1.0 / 48000.0)
    c_early = float(np.sum(f_early * early_mag) / (np.sum(early_mag) + 1e-12))
    c_late = float(np.sum(f_late * late_mag) / (np.sum(late_mag) + 1e-12))
    assert c_early > c_late + 150.0


def test_nola_formant_chants_separate_vowel_resonances():
    hey = la.render_louisiana_pad(la.LOUISIANA_16_PAD_MATRIX[9], sr=48000).mean(axis=1)   # Hey_EH (F2=1840 Hz)
    oh = la.render_louisiana_pad(la.LOUISIANA_16_PAD_MATRIX[10], sr=48000).mean(axis=1)   # Oh_OW  (F2=840 Hz)

    def f2_presence(sig: np.ndarray) -> float:
        mag = np.abs(np.fft.rfft(sig)) ** 2
        freqs = np.fft.rfftfreq(len(sig), 1.0 / 48000.0)
        hi_f2 = float(mag[(freqs >= 1500.0) & (freqs <= 2200.0)].sum())
        lo_f2 = float(mag[(freqs >= 650.0) & (freqs <= 1050.0)].sum())
        return hi_f2 / (lo_f2 + 1e-12)

    assert f2_presence(hey) > f2_presence(oh) * 2.0


def test_triggerman_bar_rolls_and_960tpq_midi_export():
    bar = la.generate_triggerman_bar(swing_amount=0.15)
    notes = {ev.midi_note for ev in bar}
    assert {36, 37, 38, 42} <= notes
    # 16 steps with 3 steps split into 1/32 pairs -> 19 hi-hat events
    hat_events = [ev for ev in bar if ev.midi_note == 42]
    assert len(hat_events) == 19

    out = Path("C:/dev/Instruments/DiskLordz_Louisiana_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = la.export_louisiana_kit(out, sr=48000, seed=504225, swing=0.15, bounce_sync_ms=18.0, make_zip=True)
    assert manifest["count"] == 16
    assert (out / "10_NOLA_Chant_Hey_EH.wav").is_file()
    assert (out / "17_NOLA_Triggerman_98BPM.mid").is_file()
    assert (out / "DiskLordz_Louisiana_Bounce_BatonRouge_Pack.zip").is_file()


if __name__ == "__main__":
    test_baton_rouge_808_and_swept_svf_boo_bass()
    test_nola_formant_chants_separate_vowel_resonances()
    test_triggerman_bar_rolls_and_960tpq_midi_export()
    print("Louisiana v2 (Formant Chant & Triggerman) tests passed")
