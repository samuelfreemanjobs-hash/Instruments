"""Contract tests for DiskLordz Houston Trunk & 12-bit Vintage Sampler Engine."""
from pathlib import Path
import shutil

import numpy as np

import forge_core_v2 as core
import houston_trunk_lab as trunk


def test_warm_trunk_808_pads_hold_exact_tuning_and_smooth_attack():
    for pad_idx in range(4):
        preset = trunk.HOUSTON_16_PAD_MATRIX[pad_idx]
        sig = trunk.render_trunk_pad(preset, sr=48000, seed=713808)
        assert np.all(np.isfinite(sig))
        assert abs(float(sig[0, 0])) < 1e-4  # 3 ms attack prevents subwoofer click
        mono = sig.mean(axis=1)
        detected, conf = core.estimate_808_pitch(mono, 48000, preset.freq_or_pitch)
        assert detected is not None and abs(12.0 * np.log2(detected / preset.freq_or_pitch)) < 0.45 and conf >= 0.5


def test_vintage_sampler_stage_rolls_off_harsh_aliasing_above_14khz():
    t = np.arange(48000 // 4) / 48000.0
    bright_noise = np.random.default_rng(11).uniform(-0.8, 0.8, len(t))
    wet = trunk.vintage_sampler_stage(bright_noise, 48000, wet_mix=1.0)
    mag_dry = np.abs(np.fft.rfft(bright_noise)) ** 2
    mag_wet = np.abs(np.fft.rfft(wet)) ** 2
    freqs = np.fft.rfftfreq(len(t), 1.0 / 48000.0)
    assert float(mag_wet[freqs > 15000.0].sum()) < float(mag_dry[freqs > 15000.0].sum()) * 0.35


def test_cowbell_pads_have_midrange_body_not_6800hz_hat_thinness():
    cowbell_low = trunk.render_trunk_pad(trunk.HOUSTON_16_PAD_MATRIX[14], sr=48000).mean(axis=1)
    cowbell_hi = trunk.render_trunk_pad(trunk.HOUSTON_16_PAD_MATRIX[15], sr=48000).mean(axis=1)
    mag = np.abs(np.fft.rfft(cowbell_low)) ** 2
    freqs = np.fft.rfftfreq(len(cowbell_low), 1.0 / 48000.0)
    mid_body = float(mag[(freqs >= 450.0) & (freqs <= 1800.0)].sum())
    ultra_air = float(mag[freqs >= 9000.0].sum())
    assert mid_body > ultra_air * 10.0

    # High cowbell fundamental sits higher than low cowbell
    def peak_freq(x: np.ndarray) -> float:
        m = np.abs(np.fft.rfft(x))
        f = np.fft.rfftfreq(len(x), 1.0 / 48000.0)
        band = (f >= 400.0) & (f <= 1400.0)
        return float(f[band][int(np.argmax(m[band]))])

    assert peak_freq(cowbell_hi) > peak_freq(cowbell_low)


def test_full_16_pad_matrix_exports_and_zips():
    out = Path("C:/dev/Instruments/DiskLordz_Houston_Trunk_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = trunk.export_houston_trunk_kit(out, sr=48000, seed=713808, make_zip=True)
    assert manifest["count"] == 16
    assert (out / "01_808_Screw_Slab_C1.wav").is_file()
    assert (out / "16_Perc_808_Cowbell_High.wav").is_file()
    assert (out / "DiskLordz_Houston_Trunk_16Pad_Pack.zip").is_file()


if __name__ == "__main__":
    test_warm_trunk_808_pads_hold_exact_tuning_and_smooth_attack()
    test_vintage_sampler_stage_rolls_off_harsh_aliasing_above_14khz()
    test_cowbell_pads_have_midrange_body_not_6800hz_hat_thinness()
    test_full_16_pad_matrix_exports_and_zips()
    print("Houston Trunk 16-pad tests passed")
