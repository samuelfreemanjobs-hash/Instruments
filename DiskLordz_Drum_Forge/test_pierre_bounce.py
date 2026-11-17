"""Contract tests for DiskLordz Cloud Bounce Drum Engine."""
import numpy as np

import forge_core_v2 as core
import pierre_bounce_lab as bounce


def test_woody_808_stays_bouncy_and_in_tune():
    p = bounce.BounceParams(sub_freq=48.9994)  # G1
    sig, root = bounce.gen_woody_808(48000, p)
    mastered = core.master(sig, 48000, -1.0)
    detected, conf = core.estimate_808_pitch(mastered, 48000, root)
    assert detected is not None and abs(12.0 * np.log2(detected / root)) < 0.45 and conf >= 0.5
    # 145 Hz woody boost increases 110-190 Hz energy relative to a flat boost=1.0 render
    flat_sig, _ = bounce.gen_woody_808(48000, bounce.BounceParams(sub_freq=48.9994, woody_mid_gain=1.0))

    def band_energy(x: np.ndarray) -> float:
        mag = np.abs(np.fft.rfft(x)) ** 2
        freqs = np.fft.rfftfreq(len(x), 1.0 / 48000.0)
        return float(mag[(freqs >= 115.0) & (freqs <= 185.0)].sum() / (mag.sum() + 1e-12))

    assert band_energy(sig) > band_energy(flat_sig)


def test_razor_clap_strips_mud_below_300hz():
    clap, _ = bounce.gen_razor_clap(48000, np.random.default_rng(5), bounce.BounceParams())
    mag = np.abs(np.fft.rfft(clap)) ** 2
    freqs = np.fft.rfftfreq(len(clap), 1.0 / 48000.0)
    low_mud = float(mag[freqs < 250.0].sum())
    snap_band = float(mag[(freqs >= 1000.0) & (freqs <= 4000.0)].sum())
    assert snap_band > low_mud * 25.0


def test_reverse_hat_has_concave_suction_and_click_free_choke():
    p = bounce.BounceParams(rev_attack_ms=110.0, rev_hold_ms=15.0, rev_choke_ms=2.2)
    rhat, _ = bounce.gen_reverse_open_hat(48000, p)
    assert np.all(np.isfinite(rhat))
    early_rms = float(np.sqrt(np.mean(rhat[: int(0.035 * 48000)] ** 2)))
    peak_rms = float(np.sqrt(np.mean(rhat[int(0.095 * 48000) : int(0.124 * 48000)] ** 2)))
    tail_max = float(np.max(np.abs(rhat[int(0.130 * 48000) :])))
    max_step = float(np.max(np.abs(np.diff(rhat))))
    assert peak_rms > early_rms * 4.0
    assert tail_max < 1e-4
    assert max_step < 0.45


if __name__ == "__main__":
    test_woody_808_stays_bouncy_and_in_tune()
    test_razor_clap_strips_mud_below_300hz()
    test_reverse_hat_has_concave_suction_and_click_free_choke()
    print("Cloud Bounce tests passed")
