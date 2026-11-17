"""Contract tests for DiskLordz HYPER DNA 2D Morph Matrix, single-pass log ratio, locks & DNA Sequencer."""
from pathlib import Path
import math
import shutil

import numpy as np

import forge_core_v2 as core
import hyper_dna_engine as dna


def test_weights_sum_to_one_and_digicore_center_dominates():
    w = dna.calculate_weights(0.51, 0.47)  # exact Digicore anchor coordinate
    assert abs(sum(w.values()) - 1.0) < 1e-6
    assert w["Digicore"] > 0.70  # matches the ~76% dominant Digicore prototype readout


def test_single_pass_pitch_start_fix_does_not_square_intensity():
    orig = dna.DrumDNA(root_hz=40.0, pitch_start_hz=80.0)  # original ratio = 2.0
    # At exact Rage coordinate (0.85, 0.14), Rage profile ratio is 190/50 = 3.8
    w = {"Plugg": 0.0, "Rage": 1.0, "Hypertrap": 0.0, "Hyperpop": 0.0, "Digicore": 0.0}
    half = dna.blend_dna(orig, w, intensity=0.5)
    expected_half = 40.0 * math.exp(math.log(2.0) + (math.log(3.8) - math.log(2.0)) * 0.5)
    assert abs(half.pitch_start_hz - expected_half) < 1e-4
    assert abs(half.root_hz - 40.0) < 1e-6  # root preserved!


def test_parameter_locks_preserve_decay_and_drive():
    orig = dna.DrumDNA(root_hz=48.0, decay_ms=610.0, drive=1.25, transient=0.22)
    w = dna.calculate_weights(0.85, 0.14)  # Rage
    locked = dna.blend_dna(
        orig,
        w,
        intensity=1.0,
        locks=dna.DNALocks(lock_root=True, lock_transient=True, lock_decay=True, lock_drive=True),
    )
    assert locked.decay_ms == orig.decay_ms
    assert locked.drive == orig.drive
    assert locked.transient == orig.transient


def test_dna_sequencer_and_evolution_tree_preserve_root_pitch():
    orig = dna.DrumDNA(root_hz=48.9994, pitch_start_hz=135.0, decay_ms=520.0)
    # Render Plugg corner vs Digicore center and verify both stay locked on G1 (49 Hz)
    plugg_sig = dna.synthesize_from_dna(dna.blend_dna(orig, dna.calculate_weights(0.12, 0.16), 0.9), "808", 48000).mean(axis=1)
    digi_sig = dna.synthesize_from_dna(dna.blend_dna(orig, dna.calculate_weights(0.51, 0.47), 0.9), "808", 48000).mean(axis=1)
    common = min(len(plugg_sig), len(digi_sig))
    assert not np.allclose(plugg_sig[:common], digi_sig[:common])
    for sig in (plugg_sig, digi_sig):
        det, conf = core.estimate_808_pitch(sig, 48000, 48.9994)
        assert det is not None and abs(12.0 * np.log2(det / 48.9994)) < 0.45 and conf >= 0.5

    out = Path("C:/dev/Instruments/DiskLordz_Hyper_DNA_Demo")
    shutil.rmtree(out, ignore_errors=True)
    manifest = dna.export_hyper_dna_demo(out, sr=48000)
    assert manifest["accepted_count"] >= 10
    assert (out / manifest["sequencer_loop"]).is_file()


if __name__ == "__main__":
    test_weights_sum_to_one_and_digicore_center_dominates()
    test_single_pass_pitch_start_fix_does_not_square_intensity()
    test_parameter_locks_preserve_decay_and_drive()
    test_dna_sequencer_and_evolution_tree_preserve_root_pitch()
    print("Hyper DNA 2D Morph Matrix & Evolution tests passed")
