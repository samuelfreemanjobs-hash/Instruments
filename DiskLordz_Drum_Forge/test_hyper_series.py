"""Contract tests for DiskLordz HYPER SERIES (20 styles, separate Hypertrap/Hyperpop, Hyper DNA Morph)."""
from pathlib import Path
import shutil

import numpy as np

import forge_core_v2 as core
import hyper_series_lab as hyper


def test_twenty_styles_and_400_pack_specs_defined():
    assert len(hyper.HYPER_20_STYLES) == 20
    assert sum(hyper.HYPERTRAP_400_SPEC.values()) == 400
    assert sum(hyper.HYPERPOP_400_SPEC.values()) == 400


def test_hypertrap_nuclear_808_preserves_48hz_sub_under_wavefolding():
    sig, root = hyper.gen_hypertrap_nuclear_808(48000, root_hz=48.9994, spec=hyper.HYPER_20_STYLES["hypertrap"])
    mastered = core.master(sig.mean(axis=1), 48000, -1.0)
    detected, conf = core.estimate_808_pitch(mastered, 48000, root)
    assert detected is not None and abs(12.0 * np.log2(detected / root)) < 0.45 and conf >= 0.5


def test_hypertrap_vs_hyperpop_distinct_sonic_signatures():
    ht_kick = hyper.gen_hyperpop_bubble_kick(48000, hyper.HYPER_20_STYLES["hypertrap"]).mean(axis=1)
    hp_kick = hyper.gen_hyperpop_bubble_kick(48000, hyper.HYPER_20_STYLES["hyperpop"]).mean(axis=1)
    assert not np.allclose(ht_kick, hp_kick)
    # Hyperpop has frequency shift + bitcrush + rubber bounce -> higher upper-spectrum centroid
    def centroid(x: np.ndarray) -> float:
        m = np.abs(np.fft.rfft(x))
        f = np.fft.rfftfreq(len(x), 1.0 / 48000.0)
        return float(np.sum(f * m) / (np.sum(m) + 1e-12))

    assert abs(centroid(hp_kick) - centroid(ht_kick)) > 80.0


def test_hyper_dna_morph_generates_five_distinct_variations_and_exports():
    morphed = hyper.render_morph_five_pack("808", sr=48000, seed=808999)
    assert set(morphed) == {"plugg", "rage", "hypertrap", "hyperpop", "digicore"}
    # Plugg vs Hyperpop morphed 808s are audibly distinct while keeping finite 48 Hz sub pitch
    assert not np.allclose(morphed["plugg"], morphed["hyperpop"])
    det_plugg, _ = core.estimate_808_pitch(morphed["plugg"].mean(axis=1), 48000, 48.0)
    det_hpop, _ = core.estimate_808_pitch(morphed["hyperpop"].mean(axis=1), 48000, 48.0)
    assert det_plugg is not None and det_hpop is not None
    assert abs(12.0 * np.log2(det_plugg / det_hpop)) < 0.5  # core pitch identity preserved!

    out = Path("C:/dev/Instruments/DiskLordz_Hyper_Series_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = hyper.export_hyper_series_showcase(out, sr=48000, seed=808999, make_zip=True)
    assert manifest["styles_count"] == 20
    assert manifest["count"] == 50  # 25 release-wave WAVs + 20 style WAVs + 5 DNA morph WAVs
    assert (out / "DiskLordz_Hyper_Series_Pack.zip").is_file()


if __name__ == "__main__":
    test_twenty_styles_and_400_pack_specs_defined()
    test_hypertrap_nuclear_808_preserves_48hz_sub_under_wavefolding()
    test_hypertrap_vs_hyperpop_distinct_sonic_signatures()
    test_hyper_dna_morph_generates_five_distinct_variations_and_exports()
    print("Hyper Series (20 styles + Hyper DNA Morph) tests passed")
