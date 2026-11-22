"""Contract tests for DiskLordz RAGE APOCALYPSE synthesis & kit factory."""
from pathlib import Path
import shutil

import numpy as np

import forge_core_v2 as core
import rage_apocalypse_lab as rage


def test_all_twelve_bass_reactor_designs_hold_musical_pitch():
    for idx, design in enumerate(rage.BASS_DESIGNS):
        midi = (28, 30, 31, 33)[idx % 4]
        audio, root_hz = rage.gen_rage_808(
            48000, np.random.default_rng(100 + idx), design=design, midi=midi, length="Med", destruction="Destroyed"
        )
        assert np.all(np.isfinite(audio)), design
        mastered = core.master(audio, 48000, -1.12)
        assert core.true_peak_db(mastered) <= -1.05, design
        assert abs(float(np.mean(mastered))) < 1e-3, design
        detected, conf = core.estimate_808_pitch(mastered, 48000, root_hz)
        assert detected is not None and abs(12.0 * np.log2(detected / root_hz)) < 0.5 and conf >= 0.45, (
            design,
            root_hz,
            detected,
            conf,
        )


def test_clean_mixed_destroyed_add_upper_harmonics_without_killing_sub():
    energies = []
    sub_levels = []
    for mode in ("Clean", "Mixed", "Destroyed"):
        sig, _ = rage.gen_rage_808(48000, np.random.default_rng(7), design="Chaos", midi=28, destruction=mode)
        sub = core.filter_signal(sig, 95.0, "lowpass", 48000, 4)
        high = core.filter_signal(sig, 180.0, "highpass", 48000, 3)
        sub_levels.append(float(np.sqrt(np.mean(sub ** 2))))
        energies.append(float(np.sqrt(np.mean(high ** 2))) / (sub_levels[-1] + 1e-9))
    assert energies[0] < energies[1] < energies[2]
    assert min(sub_levels) > 0.05


def test_velocity_changes_hat_brightness_not_only_gain():
    soft, _ = rage.gen_rage_hat(48000, np.random.default_rng(12), is_open=False, velocity=0.25)
    hard, _ = rage.gen_rage_hat(48000, np.random.default_rng(12), is_open=False, velocity=1.0)

    def centroid(x: np.ndarray) -> float:
        mag = np.abs(np.fft.rfft(x * np.hanning(len(x))))
        freqs = np.fft.rfftfreq(len(x), 1.0 / 48000.0)
        return float(np.sum(freqs * mag) / (np.sum(mag) + 1e-12))

    assert centroid(hard) > centroid(soft) + 400.0


def test_demo_pack_exports_commercial_folder_tree():
    out = Path("C:/dev/Instruments/DiskLordz_Rage_Apocalypse_Demo")
    shutil.rmtree(out, ignore_errors=True)
    summary = rage.build_apocalypse_pack(out, sr=48000, bits=24, seed=666808, tier="demo", ceiling_db=-1.0)
    assert summary["one_shots"] >= 20
    assert summary["midi_grooves"] == 6
    assert (out / "01_808_BASS").is_dir()
    assert (out / "12_MIDI_GROOVES").is_dir()
    assert (out / "15_JUCE_SYNTH_PRESETS" / "RageBankRecipes.json").is_file()
    assert (out / "16_DRUM_KIT_MAPPINGS" / "PadBankA_16.json").is_file()
    assert (out / "MANIFEST.json").is_file()


if __name__ == "__main__":
    test_all_twelve_bass_reactor_designs_hold_musical_pitch()
    test_clean_mixed_destroyed_add_upper_harmonics_without_killing_sub()
    test_velocity_changes_hat_brightness_not_only_gain()
    test_demo_pack_exports_commercial_folder_tree()
    print("Rage Apocalypse tests passed")
