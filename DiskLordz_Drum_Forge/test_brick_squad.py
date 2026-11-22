"""Contract tests for DiskLordz Heavy ATL / Brick Trap Drum Engine v2."""
from pathlib import Path
import shutil

import numpy as np

import brick_squad_lab as brick
import forge_core_v2 as core


def test_brick_808_resets_phase_and_holds_root_pitch():
    p = brick.BrickParams(base_freq=48.9994, drive=4.5, decay_sec=1.4)
    first, root = brick.gen_brick_808(48000, p)
    second, _ = brick.gen_brick_808(48000, p)
    assert np.array_equal(first, second)

    mastered = core.master(first, 48000, -1.0)
    detected, conf = core.estimate_808_pitch(mastered, 48000, root)
    assert detected is not None and abs(12.0 * np.log2(detected / root)) < 0.45 and conf >= 0.5
    assert abs(float(np.mean(mastered))) < 1e-3


def test_snare_clap_true_stereo_mid_scoop_and_4500hz_shelf():
    bright, _ = brick.gen_snare_clap_layer(48000, np.random.default_rng(9), brick.BrickParams(high_shelf_db=5.5))
    flat, _ = brick.gen_snare_clap_layer(48000, np.random.default_rng(9), brick.BrickParams(high_shelf_db=0.0))
    assert bright.ndim == 2 and bright.shape[1] == 2
    # Independent stereo noise ensures L and R are not identical copies
    assert not np.allclose(bright[:, 0], bright[:, 1])

    def hi_ratio(x: np.ndarray) -> float:
        mono = x.mean(axis=1)
        mag = np.abs(np.fft.rfft(mono)) ** 2
        freqs = np.fft.rfftfreq(len(mono), 1.0 / 48000.0)
        return float(mag[freqs >= 4500.0].sum() / (mag.sum() + 1e-12))

    assert hi_ratio(bright) > hi_ratio(flat)


def test_909_hat_attack_ramp_and_zip_kit_export():
    closed_hat, _ = brick.gen_909_metallic_hat(48000, np.random.default_rng(3), brick.BrickParams(), is_open=False)
    open_hat, _ = brick.gen_909_metallic_hat(48000, np.random.default_rng(3), brick.BrickParams(), is_open=True)
    assert abs(float(closed_hat[0])) < 1e-4  # 1.5 ms attack ramp prevents sample-0 click
    assert len(open_hat) > len(closed_hat) * 2

    out = Path("C:/dev/Instruments/DiskLordz_Heavy_ATL_Brick_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = brick.build_brick_kit(out, sr=48000, variations=2, seed=1017808, make_zip=True)
    assert manifest["count"] == 10
    assert (out / "DiskLordz_Heavy_ATL_Brick_Pack.zip").is_file()


if __name__ == "__main__":
    test_brick_808_resets_phase_and_holds_root_pitch()
    test_snare_clap_true_stereo_mid_scoop_and_4500hz_shelf()
    test_909_hat_attack_ramp_and_zip_kit_export()
    print("Heavy ATL Brick v2 tests passed")
